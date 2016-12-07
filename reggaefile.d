import reggae;

enum CompileCommand {
	Compile = "g++ -c -std=c++11 -O0 -ggdb -Wall $in -o $out",
	Link = "g++ -std=c++11 -O3 -ggdb -Wall -lSDL2 -lGL -lGLEW $in -o $out",
}

Target[] MakeObjects(string[] files, string[] headers) {
	Target[] objs;
	Target[] head;

	foreach (h; headers)
		head ~= Target("src/"~h~".hpp");

	foreach (f; files)
		objs ~= Target(f~".o", CompileCommand.Compile, [Target("src/"~f~".cpp")], head);
	
	return objs;
}

enum objs = MakeObjects(["main", "engine", "scopeexit", "shader", "mesh"], ["engine", "scopeexit", "shader", "mesh"]);

enum raycast = Target("turtlegl", CompileCommand.Link, objs);

mixin build!(raycast);
