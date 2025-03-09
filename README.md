# Open Unreal Utiltiies - Gameplay Tags

Various gameplay tag extensions for Unreal Engine 5:

- Literal Tags: Native tag declarations as C++ structs that allow auto completion, etc instead of relying on string literals
- Typed Tags: Typesafe gameplay tags and tag containers for properties
- Tag Query Parser: Parse gameplay tag queries from simple input strings (for debug tools etc)
- Tag Dependencies: Link tag providers together to form a dependency hierarchy

## Blog

For more info on what you can do with literal and typed tags, please refer to this blog post: https://jonasreich.de/blog/006-managing-gameplay-tag-complexity.html

## Production Ready?
Most of these tag utilities, first and foremost the literal and typed tags, were used for many gameplay systems in [Titan Quest II](https://store.steampowered.com/app/1154030/Titan_Quest_II/) developed by [Grimlore Games](https://grimloregames.com/).

## Sister Plugins

All utils in this plugin were developed as part of the [Open Unreal Utilities](https://github.com/JonasReich/OpenUnrealUtilities) but extracted to its own plugin, because they proved incredibly useful and after some small refactoring could be separated from the other components in that plugin.
