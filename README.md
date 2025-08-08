# Open Unreal Utilities - Gameplay Tags

Various extensions to [Gameplay Tags](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-gameplay-tags-in-unreal-engine?application_version=5.2) in Unreal Engine 5.

- Literal Tags: Declare C++ structs instead of relying on string literals when using tags in source code
- Typed Tags: Typesafe gameplay tags and tag containers for editor properties
- Tag Query Parser: Parse gameplay tag queries from simple input strings (for debug tools etc)
- Tag Dependencies: Link tag providers together to form a dependency hierarchy
- Tag Audits: Analyze Tag Usages to find unused tags and most commonly used tags in your tag hierarchy
- Tag Validation: Enforce rules for tag creation to maintain a clean tag tree

## Blog

For more info on what you can do with literal and typed tags, please refer to this blog post: https://jonasreich.de/blog/006-managing-gameplay-tag-complexity.html

## Production Ready?
Most of these tag utilities, first and foremost the literal and typed tags, were used for many gameplay systems in [Titan Quest II](https://store.steampowered.com/app/1154030/Titan_Quest_II/) developed by [Grimlore Games](https://grimloregames.com/).

## Sister Plugins

All utils in this plugin were developed as part of the [Open Unreal Utilities](https://github.com/JonasReich/OpenUnrealUtilities) but extracted to its own plugin, because they proved incredibly useful and after some small refactoring could be separated from the other components in that plugin.
