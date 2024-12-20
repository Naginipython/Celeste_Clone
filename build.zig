const std = @import("std");

const PROGRAM_NAME: []const u8 = "main";

fn linker(exe: *std.Build.Step.Compile, files: []const []const u8, b: *std.Build, target: std.Build.ResolvedTarget) void {
    exe.addCSourceFiles(.{
        .files = files,
        .flags = &[_][]const u8{"-Wall"},
    });
    exe.addIncludePath(b.path("include"));
    exe.linkLibC();
    exe.linkLibCpp();

    // Libs
    if (target.query.isNativeOs() and target.result.os.tag == .windows) {
        // todo
        const sdl_dep = b.dependency("SDL", .{ // Add libs as needed
            .target = target,
        });
        const sdl_img_dep = b.dependency("SDL_image", .{
            .target = target,
        });
        exe.linkLibrary(sdl_dep.artifact("SDL2"));
        exe.linkLibrary(sdl_img_dep.artifact("SDL2_image"));
        // TODO: glew
    } else {
        exe.linkSystemLibrary("SDL2"); // Add libs as needed
        exe.linkSystemLibrary("SDL2_image");
        exe.linkSystemLibrary("glew");
    }
}

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});

    // Main build
    const files = try findFiles("src", &[_][]const u8{"dll"});
    std.debug.print("Files built:\n{s}\n", .{files});

    const exe = b.addExecutable(.{ .name = PROGRAM_NAME, .target = target });
    linker(exe, files, b, target);

    b.installArtifact(exe);

    // dll
    const dll_files = try findFiles("src", &[_][]const u8{"main"});
    // const dll_files = &[_][]const u8{"src/dll-game.cpp", "src/window.cpp", "src/opengl.cpp", "src/fileIO.cpp"};
    const dll = b.addSharedLibrary(.{ .name = "game", .target = target, .optimize = b.standardOptimizeOption(.{}) });
    dll.addCSourceFiles(.{
        .files = dll_files,
        .flags = &[_][]const u8{},
    });
    dll.addIncludePath(b.path("include"));
    dll.linkLibC();
    dll.linkLibCpp();
    b.installArtifact(dll);

    // ------ Run ------
    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);

    // ------ Tests ------
    // const test_files = try findFiles("tests", &[_][]const u8{"main.cpp"});
    // std.debug.print("Test Files built:\n{s}\n", .{test_files});

    // // combine with `files`, except src/main.c or src/main.cpp
    // const tests_name = PROGRAM_NAME ++ "_tests";
    // const tests_exe = b.addExecutable(.{ .name = tests_name, .target = target });

    // linker(tests_exe, test_files, b, target);
    // const googletest_dep = b.dependency("googletest", .{
    //     .target = target,
    // });
    // tests_exe.linkLibrary(googletest_dep.artifact("gtest"));

    // b.installArtifact(tests_exe);

    // const run_tests_exe = b.addRunArtifact(tests_exe);
    // run_tests_exe.step.dependOn(b.getInstallStep());

    // const test_step = b.step("test", "Run unit tests");
    // test_step.dependOn(&run_tests_exe.step);

    // ------ Clean ------
    const clean_step = b.step("clean", "Clean the directory");
    // Windows
    // clean_step.dependOn(&b.addRemoveDirTree(b.path("zig-out")).step);
    // clean_step.dependOn(&b.addRemoveDirTree(b.path(".zig-cache")).step);
    // Linux
    clean_step.dependOn(&b.addRemoveDirTree(b.install_path).step);
    clean_step.dependOn(&b.addRemoveDirTree(b.pathFromRoot(".zig-cache")).step);
}

// TODO: Add ignore word
fn findFiles(src: []const u8, ignore_list: []const []const u8) ![]const []const u8 {
    var result = std.ArrayList([]const u8).init(std.heap.page_allocator);
    // todo: error handling for root (for test dir)
    var root = try std.fs.cwd().openDir(src, .{ .iterate = true });
    defer root.close();

    var iter = root.iterate();
    while (try iter.next()) |entry| {
        // ignore if on ignore list
        var ignore = false;
        for (ignore_list) |item|
            if (std.mem.indexOf(u8, entry.name, item) != null) {
                ignore = true;
                break;
            };
        if (ignore) {
            continue;
        }

        // Create item
        var item = std.ArrayList(u8).init(std.heap.page_allocator);
        try item.appendSlice(src);
        try item.append('/');
        try item.appendSlice(entry.name);
        if (entry.kind == .file) {
            const check_cpp = entry.name[entry.name.len - 4 ..];
            const check_c = entry.name[entry.name.len - 2 ..];
            if (std.mem.eql(u8, check_cpp, ".cpp") or std.mem.eql(u8, check_c, ".c")) {
                const path_u8 = try item.toOwnedSlice();
                try result.append(path_u8);
            }
        }
        if (entry.kind == .directory) {
            const dir_u8 = try item.toOwnedSlice();
            const files = try findFiles(dir_u8, ignore_list);
            for (files) |f|
                try result.append(f);
        }
    }
    const res = try result.toOwnedSlice();
    return res;
}
