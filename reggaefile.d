import reggae;

enum CompileCommand {
	Compile = "g++ -c -std=c++11 -O0 -ggdb -Wall -fdiagnostics-color=always $in -o $out",
	Link = "g++ -std=c++11 -O3 -ggdb -Wall -fdiagnostics-color=always -lSDL2 -lSDL2_image -lGL -lGLEW -lassimp $in -o $out",
}

Target[] MakeObjects(string[] files) {
	import std.file : dirEntries, SpanMode;
	import std.process : executeShell;
	import std.algorithm : map;
	import std.array : array, replace, split;

	Target[] objs;

	foreach (f; dirEntries("src/", "*.cpp", SpanMode.breadth).filter!(x => !x.isDir)) {
		auto dep = executeShell("g++ -MM " ~ f);
		if (dep.status)
			assert(0, dep.output);

		auto head = dep.output.split(":")[1].replace("\n", " ").split(" ").filter!(s => !s.empty && s != "\\").map!(x => Target(x)).array;
		objs ~= Target(f ~ ".o", CompileCommand.Compile, [Target(f)], head);
	}

	return objs;
}

Build myBuild() {
	auto objs = MakeObjects(["main.cpp", "engine.cpp", "scopeexit.cpp", "shader.cpp", "mesh.cpp"]);

	auto raycast = Target("turtlegl", CompileCommand.Link, objs);

	return Build(raycast);
}
