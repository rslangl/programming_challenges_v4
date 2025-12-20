const std = @import("std");

pub fn main() !void {
    var args = std.process.args();

    // skips argv[0]
    _ = args.next();

    if (args.next()) |input| {
        const out = std.io.getStdOut().writer();

        const limit = try std.fmt.parseUnsigned(u16, input, 10);
        std.debug.print("Input: {d}\n", .{limit});

        for (0..limit) |n| {

            if (n % 15 == 0) {
                try out.print("{s}\n", .{"fizzbuzz"});
            } else if (n % 5 == 0) {
                try out.print("{s}\n", .{"fizz"});
            } else if (n % 3 == 0) {
                try out.print("{s}\n", .{"buzz"});
            } else {
                try out.print("{d}\n", .{n});
            }
        }

    } else {
        const out = std.io.getStdErr().writer();
        try out.print("error: {s}\n", .{"a numeric input is required"});
    }
}
