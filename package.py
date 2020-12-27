import argparse
import glob
import io
import itertools
import json
import os
import zipfile

os.chdir(os.path.dirname(os.path.abspath(__file__)))

EXCLUDED = [
	"*.json",
	"requirements.txt",
	"**.zip",
	"**.py"
]

def get_package_info():
	package_name = None
	package_version = None
	package_description = None
	boards = []
	with open("platform.txt") as f:
		for line in f.readlines():
			splits = line.strip().split("=", 2)
			key = splits[0]
			value = splits[-1]
			if key == "package":
				package_name = value
			elif key == "version":
				package_version = value
			elif key == "name":
				package_description = value
	with open("boards.txt") as f:
		for line in f.readlines():
			splits = line.strip().split("=", 2)
			key = splits[0]
			value = splits[-1]
			if key.endswith(".name"):
				boards.append(value)
	if package_name is None:
		raise RuntimeError("Cannot get name of package (package=... in platform.txt)")
	if package_version is None:
		raise RuntimeError("Cannot get version of package (version=... in platform.txt)")
	if package_description is None:
		raise RuntimeError("Cannot get description of package (name=... in platform.txt)")
	if len(boards) == 0:
		raise RuntimeError("No boards found")
	return {
		"name": package_name,
		"version": package_version,
		"description": package_description,
		"boards": boards
	}

def gen_package_index(out_file, package_url):
	with open("package_index_template.json") as f:
		template = json.load(f)
	pkg_info = get_package_info()
	platform = template["packages"][0]["platforms"][0]
	platform["name"] = pkg_info["description"]
	platform["version"] = pkg_info["version"]
	platform["url"] = package_url
	platform["archiveFileName"] = package_url.split("/")[-1]
	platform["boards"] = [{"name": b} for b in pkg_info["boards"]]
	json.dump(template, out_file)


def gen_package(out_file):
	pkg_info = get_package_info()
	all_files = glob.glob("**", recursive=True)
	excluded_files = list(itertools.chain(*[glob.glob(exclude, recursive=True) for exclude in EXCLUDED]))
	included_files = [f for f in all_files if f not in excluded_files]
	with zipfile.ZipFile(out_file, "x") as z:
		for filename in included_files:
			z.write(filename, os.path.join(f"{pkg_info['name']}-{pkg_info['version']}", filename))


def serve(port: int, package_name: str):
	import bottle

	b = bottle.Bottle()
	package_path = f"/{package_name}.zip"

	@b.route("/")
	def index():
		return "Development Server"

	@b.route(f"/package_{package_name}_index.json")
	def package_index():
		package_url = "/".join(bottle.request.url.split("/")[:-1]) + package_path
		out_file = io.StringIO()
		gen_package_index(out_file, package_url)
		bottle.response.content_type = "application/json"
		return out_file.getvalue()

	@b.route(package_path)
	def package():
		out_file = io.BytesIO()
		gen_package(out_file)
		bottle.response.content_type = "application/zip"
		return out_file.getvalue()

	b.run(port=port)


def main():
	parser = argparse.ArgumentParser()

	pkg_info = get_package_info()

	parser.add_argument("--serve", action="store_true")
	parser.add_argument("--port", type=int, default=58080)
	parser.add_argument("--index", default=f"package_{pkg_info['name']}_index.json")
	parser.add_argument("--package", default=f"package_{pkg_info['name']}_{pkg_info['version']}.zip")
	parser.add_argument("--package-url", type=str)

	args = parser.parse_args()
	if args.serve:
		serve(args.port, pkg_info['name'])
	else:
		if args.package_url is None:
			raise RuntimeError("Need to specify --package-url")
		with open(args.index, "w") as f:
			gen_package_index(f, args.package_url)
		with open(args.package, "wb") as f:
			gen_package(f)


if __name__ == "__main__":
	main()
