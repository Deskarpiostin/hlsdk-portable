# Half-Life SDK for GoldSource and Xash3D [![Build Status](https://github.com/FWGS/hlsdk-portable/actions/workflows/build.yml/badge.svg?branch=master)](https://github.com/FWGS/hlsdk-portable/actions/workflows/build.yml) [![Windows Build Status](https://ci.appveyor.com/api/projects/status/github/FWGS/hlsdk-portable?svg=true)](https://ci.appveyor.com/project/a1batross/hlsdk-portable)

Half-Life SDK for GoldSource & Xash3D with some bugfixes.

<details><summary>Changelog</summary>
<p>

- Fixed an occasional bug when houndeyes stuck unable to do anything. Technical detail: now monster's `Activity` is set before the call to `SetYawSpeed`. [Patch](https://github.com/FWGS/hlsdk-portable/commit/467899b99aa225a95d90222137f18c141c929c86)
- Monsters now play idle sounds as it's supposed by the code. Technical detail: the problem was a check for a wrong variable. [Patch](https://github.com/FWGS/hlsdk-portable/commit/9fc712da019a1ca646171e912209a993e7c43976)
- Fixed a bug that caused talk monsters (scientists and security guards) to face a wrong direction during scripted sequence sometimes. [Patch](https://github.com/FWGS/hlsdk-portable/commit/3e2808de62e479e83068c075cb88b4f177f9acc7)
- Fixed squad member removal. This bug affected houndeye attacks as their attack depends on percieved number of squad members. [Patch](https://github.com/FWGS/hlsdk-portable/commit/b4502f71336a08f3f2c72b7b061b2838a149a11b)
- Scientists now react to smells. [Patch](https://github.com/FWGS/hlsdk-portable/commit/2de4e7ab003d5b1674d12525f5aefb1e57a49fa3)
- Tau-cannon (gauss) plays idle animations.
- Tau-cannon (gauss) beam color depends on the charge as it was before the prediction code was introduced in Half-Life. [Patch](https://github.com/FWGS/hlsdk-portable/commit/0a29ec49c8183ebb8da22a6d2ef395eae9c3dffe)
- Brought back gluon flare in singleplayer. [Patch](https://github.com/FWGS/hlsdk-portable/commit/9d7ab6acf46a8b71ef119d9c252767865522d21d)
- Hand grenades don't stay primed after holster, preventing detonation after weapon switch. [Patch](https://github.com/FWGS/hlsdk-portable/commit/6e1059026faa90c5bfe5e3b3f4f58fde398d4524)
- Fixed flashlight battery appearing as depleted on restore.
- Fixed a potential overflow when reading sentences.txt. [Patch](https://github.com/FWGS/hlsdk-xash3d/commit/cb51d2aa179f1eb622e08c1c07b053ccd49e40a5)
- Fixed beam attachment invalidated on restore (that led to visual bugs). [Patch](https://github.com/FWGS/hlsdk-xash3d/commit/74b5543c83c5cdcb88e9254bacab08bc63c4c896)
- Fixed alien controllers facing wrong direction in non-combat state. [Patch](https://github.com/FWGS/hlsdk-portable/commit/e51878c45b618f9b3920b46357545cbb47befeda)
- Fixed weapon deploy animations not playing sometimes on fast switching between weapons. [Patch](https://github.com/FWGS/hlsdk-portable/commit/ed676a5413c2d26b2982e5b014e0731f0eda6a0d) [Patch2](https://github.com/FWGS/hlsdk-portable/commit/4053dca7a9cf999391cbd77224144da207e4540b)
- Fixed tripmine sometimes having wrong body on pickup [Patch](https://github.com/FWGS/hlsdk-portable/commit/abf08e4520e3b6cd12a40f269f4a256cf8496227)

Bugfix-related macros that can be enabled during the compilation:

- **CROWBAR_DELAY_FIX** fixes a bug when crowbar has a longer delay after the first hit.
- **CROWBAR_FIX_RAPID_CROWBAR** fixes a "rapid crowbar" bug when hitting corpses of killed monsters.
- **GAUSS_OVERCHARGE_FIX** fixes tau-cannon (gauss) charge sound not stopping after the overcharge.
- **CROWBAR_IDLE_ANIM** makes crowbar play idle animations.
- **TRIPMINE_BEAM_DUPLICATION_FIX** fixes tripmine's beam duplication on level transition.
- **HANDGRENADE_DEPLOY_FIX** makes handgrenade play draw animation after finishing a throw.
- **WEAPONS_ANIMATION_TIMES_FIX** fixes deploy and idle animation times of some weapons.

HL25-related macros that can be enabled during the compilation:

- **SATCHEL_OLD_BEHAVIOUR** old pre-HL 25th satchel's behaviour.
- **SPEAKABLE_TARGETS** makes speakable cycler and func_button(and breaks amxmodx plugins).

Bugfix-related server cvars:

- **satchelfix**: if set to 1, doors won't get blocked by satchels. Fixes an infamous exploit on `crossfire` map.
- **explosionfix**: if set to 1, explosion damage won't propagate through thin bruses.
- **selfgauss**: if set to 0, players won't hurt themselves with secondary attack when shooting thick brushes.

*Note*: the macros and cvars were adjusted in [hlfixed](https://github.com/FWGS/hlsdk-portable/tree/hlfixed) branch (for further information read [this](https://github.com/FWGS/hlsdk-portable/wiki/HL-Fixed)). The bugfix macros are kept turned off in `master` branch to maintain the compatibility with vanilla servers and clients.

Other server cvars:

- **mp_bhopcap**: if set to 0, disable bunny-hop restriction.
- **chargerfix**: if set to 1, wall-mounted health and battery chargers will play reject sounds if player has the full health or armor.
- **corpsephysics**: if set to 1, corpses of killed monsters will fly a bit from an impact. It's a cut feature from Half-Life.

</p>
</details>

<details><summary>Support for mods</summary>
<p>

This repository contains (re-)implementations of some mods as separate branches derived from `master`. The list of supported mods can be found [here](https://github.com/FWGS/hlsdk-portable/wiki/Mods). Note that some branches are unstable and incomplete.

To get the mod branch locally run the following git command:

```
git fetch origin asheep:asheep
```

This is considering that you have set **FWGS/hlsdk-portable** as an `origin` remote and want to fetch `asheep` branch.

</p>
</details>

# Obtaining source code

Either clone the repository via [git](`https://git-scm.com/downloads`) or just download ZIP via **Code** button on github. The first option is more preferable as it also allows you to search through the repo history, switch between branches and clone the vgui submodule.

To clone the repository with git type in Git Bash (on Windows) or in terminal (on Unix-like operating systems):

```
git clone --recursive https://github.com/deskarpiostin/hlsdk-portable
```

# Android
1. Set up [Android Studio/Android SDK](https://developer.android.com/studio).

### Android Studio
Open the project located in the `android` folder and build.

### Command-line
```
cd android
./gradlew assembleRelease
```

### Customizing the build
settings.gradle:
* **rootProject.name** - project name displayed in Android Studio (optional).

app/build.gradle:
* **android->namespace** and **android->defaultConfig->applicationId** - set both to desired package name.
* **getBuildNum** function - set **releaseDate** variable as desired.

app/java/su/xash/hlsdk/MainActivity.java:
* **.putExtra("gamedir", ...)** - set desired gamedir.

src/main/AndroidManifest.xml:
* **application->android:label** - set desired application name.
* **su.xash.engine.gamedir** value - set to same as above.

## Other platforms

Building on other architectures (e.g x86_64 or arm) and POSIX-compliant OSes (e.g. FreeBSD) is supported.

### Prerequisites

Install C and C++ compilers (like gcc or clang), cmake and make.

### Building

```
cmake -DCMAKE_BUILD_TYPE=Release -B build -S .
cmake --build build
```

Force 64-bit build:
```
cmake -DCMAKE_BUILD_TYPE=Release -D64BIT=1 -B build -S .
cmake --build build
```

### Building with waf

To use waf, you need to install python (2.7 minimum)

```
./waf configure -T release
./waf
```

Force 64-bit build:
```
./waf configure -T release -8
./waf
```

## Build options

Some useful build options that can be set during the cmake step.

* **GOLDSOURCE_SUPPORT** - allows to turn off/on the support for GoldSource input. Set to **ON** by default on x86 Windows and x86 Linux, **OFF** on other platforms.
* **64BIT** - allows to turn off/on 64-bit build. Set to **OFF** by default on x86_64 Windows, x86_64 Linux and 32-bit platforms, **ON** on other 64-bit platforms.
* **USE_VGUI** - whether to use VGUI library. **OFF** by default. You need to init `vgui_support` submodule in order to build with VGUI.

This list is incomplete. Look at `mod_options.txt` to see all available options and their default values.

Prepend option names with `-D` when passing to cmake. Boolean options can take values **OFF** and **ON**. Example:

```
cmake .. -DUSE_VGUI=ON -DGOLDSOURCE_SUPPORT=ON -DCROWBAR_IDLE_ANIM=ON -DCROWBAR_FIX_RAPID_CROWBAR=ON
```

To add new build options for your mod, you can add them to `mod_options.txt` file in the following format:
```
<definition name>=<definition value> # <description>
```
If `definition value` set to `OFF` or `ON`, it will be considered as a boolean value. Otherwise it will be a string. Nor `definition name` nor `definition value` can have whitespace characters.
