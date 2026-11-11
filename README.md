# Command & Conquer: Generals + Zero Hour — Android

<img src="assets/android-screenshot.png" alt="Generals Zero Hour main menu running on Android tablet" width="600">

**Both 2003 RTS engines running natively on Android tablets** — not emulation,
not a compatibility layer. The real C++ engines compiled for arm64, rendering
DirectX 8 → [DXVK](https://github.com/doitsujin/dxvk) → Vulkan on Adreno/Mali GPUs.
The Android release uses d3d8to9 and DXVK Native 1.9.2b.

**Generals and Zero Hour both ship in one APK** and are chosen in the launcher —
they are two separate engines (`libmain.so` and `libmain_generals.so`), not one
engine with different data.

> ⚠️ **You must own a legal copy of the game.** No game assets are included or
> distributed. See [How to Play](#how-to-play) below.

---

## Maintenance status

Active maintenance has ended. **v0.12 is the latest published release** and is kept available for community testing and experimentation. Device compatibility and feature support vary; consult the status table below for what has actually been tested. No ongoing fixes or support are promised. Forks and community contributions are welcome. See the [final handover](docs/WORKDIR/reports/FINAL_MAINTENANCE_HANDOVER.md) for preserved technical context and validation notes.

## Status

| Feature | Zero Hour | Generals |
|---------|:---------:|:--------:|
| Engine init (all subsystem stores) | ✅ | ✅ |
| DXVK D3D8→Vulkan rendering | ✅ | ✅ |
| Main menu renders with text | ✅ | ✅ |
| Native resolution (no letterboxing) | ✅ | ✅ |
| Terrain rendering | ✅ | ✅ |
| Touch input (RTS gesture set) | ✅ | ✅ |
| Mouse input (movement, buttons, edge scroll) | ✅ | shared code, untested |
| Audio playback | ✅ music and video audio | shared code, untested |
| Video playback (Bink intro + cutscenes) | ✅ | shared code, untested |
| Full gameplay session (skirmish) | ✅ | untested |
| Campaign missions | ✅ | reaches mission intro |
| Multiplayer | ❌ untested | ❌ untested |

**Mods** load from `.gib` archives, so GenLauncher-format mods work. Rise of the
Reds is verified playable: its own menus, factions, units and skirmish. ShockWave,
Project Raptor, Generals Continue and Shockwave Chaos ship the same way and should
import, but have not been run.

> **Limitation:** the engine reads only **archives and videos** from a mod
> directory (`-mod`). Loose files a mod ships outside an archive are copied by the
> importer but are not on the engine's search path — most notably
> `Data/Scripts/SkirmishScripts.scb`, the compiled skirmish AI. Mods that ship it
> loose (Generals Continue, Project Raptor) will install and run, but their AI
> opponents may not behave correctly. Mods that pack everything into `.gib`
> (Rise of the Reds, ShockWave) are unaffected.

**Shadow volumes are disabled by default** (a launcher toggle). They draw as
opaque black geometry through DXVK on Mali, which is what most "black terrain"
and "black model" reports were. Shadow decals are used instead and render
correctly. The Rise of the Reds DXT3 black-building bug was fixed in v0.10 --
see [the historical investigation](docs/port/KNOWN_ISSUE_BLACK_MODELS.md).

Generals is newer than Zero Hour here: it boots, renders at native resolution
with correct terrain, and reaches a campaign mission intro. Everything marked
"shared code" runs through the same files Zero Hour uses, so it is expected to
work, but has not been separately confirmed on device.

**Audio note.** Music and video audio are confirmed playing on device. Mission
speech/EVA commentary goes through the same decoder path, which was fixed in the
same change and verified at the object level, but has not been separately
confirmed by ear in a mission.

### Historical device coverage

| Device | GPU / driver | Renderer | Result |
|---|---|---|---|
| OnePlus Pad 2 | Adreno 830, Vulkan 1.3 | Historical DXVK lane | Full gameplay verified |
| TCL NXTPAPER 9469X | Mali-G57 MC2, Vulkan 1.1.177 | Mali legacy lane | Both games. Zero Hour: full mission, 30 FPS, video/audio/touch/mouse verified. Generals: boots to its own menu, native resolution, terrain renders, reaches a mission intro |
| Galaxy S24 Ultra | Adreno 750, Vulkan 1.3 | Historical DXVK lane | Runs; video and audio verified |

These are historical port results, not final-candidate test results. v0.12 was
tested on TCL; its S24 installation had no game data. See the release-specific
evidence in [the handover](docs/WORKDIR/reports/FINAL_MAINTENANCE_HANDOVER.md).

The original TCL result was measured on physical hardware on 2026-07-30. Text, icons,
colors, animated menu rendering, touch input, and mission gameplay are correct.
A complete mission was played without a crash or rendering failure. Extended
replay, suspend/resume, thermal, and memory-growth testing remain useful
follow-up coverage.

---

## How to Play

### What you need

1. **An Android tablet** with:
   - **arm64-v8a** architecture (all modern tablets)
   - **Android 7.0+** (API 24+, for system Vulkan support)
   - A **Vulkan-capable GPU** (modern Adreno, or a supported Mali device)
   - **~3GB free RAM** for the game process
   - **~2.5GB storage** for game data

2. **A legal copy of C&C Generals Zero Hour**:
   - [Steam](https://store.steampowered.com/app/2732960/) (check the store listing for edition contents)
   - Or any retail/EA App/Origin install
   - You need the **Complete Edition** or both Generals + Zero Hour installed

### Step 1: Download the APK

Grab the latest APK from the [**Releases page**](../../releases):

Since v0.11, **one APK serves both tested Adreno and Mali devices**:
`GeneralsZH-vX.Y.apk`, using d3d8to9 + DXVK Native 1.9.2b. The older
`-Vulkan` / `-TCL-Mali` split is retired. DXVK 2.6 caused crashes on the tested
Adreno device and must not be substituted just because a GPU supports Vulkan 1.3.
Compatibility with every Vulkan-capable device is not established.

> **Do not uninstall to upgrade.** Uninstalling deletes
> `Android/data/me.generalsx.zh/`, including your copied GameData. Install over
> the top with `adb install -r`; the releases are signed consistently so this
> works.

### Step 2: Install the APK

```bash
# Enable "Install from unknown sources" for your file manager first
adb install -r GeneralsZH-v0.12.apk

# Or transfer the APK to your tablet and tap it in Files
```

### Step 3: Copy your game data

Copy your legally owned game install to a folder accessible through Android's
file picker, then choose **Import game files…** in the launcher. Select the folder
containing the archives; keep original filenames and the install's directory
structure. The importer copies the supported files into app storage. Fonts are
bundled and extracted automatically.

Use **Verify game files** after import. This checks archive headers and sizes;
a clean report does not prove all required game content is present or valid.
If launch fails, use **Export engine log** and retain both available sessions.

**Required .big files** (include the archives from your game installation):

| File | Contents |
|------|----------|
| `INI.big` | Base game INI data (weapons, objects, etc.) |
| `INIZH.big` | Zero Hour INI data |
| `Textures.big` / `TexturesZH.big` | Game textures |
| `Audio.big` / `AudioZH.big` | Sound effects |
| `Music.big` / `MusicZH.big` | Music tracks |
| `MapsZH.big` | Map data |
| `Terrain.big` / `TerrainZH.big` | Terrain data |
| `W3D.big` / `W3DZH.big` | 3D models |
| `English.big` / `EnglishZH.big` | English text/speech |
| `Speech*.big` | Voice-over files |
| `Window.big` / `WindowZH.big` | UI textures |
| `ShadersZH.big` | Shaders |

### Step 4: Play

Launch **"Generals ZH"** from your app drawer. The launcher opens first.

---

## The Launcher

Opening the app shows a launcher rather than starting the game directly. It
handles the things that previously needed a PC and `adb`:

| | |
|---|---|
| **Game data** | Pick which installed data set to run. Your existing `GameData` folder is detected automatically — nothing to migrate. |
| **Import game files…** | Copy a game install straight from the tablet's storage (or an SD card / USB drive) with a folder picker and a progress bar. No PC needed. |
| **Mod** | Select an installed mod, applied over the chosen game data. |
| **Install mod…** | Import a mod folder the same way, naming it as you go. |
| **Skip intro videos** | Straight to the menu — **5 seconds instead of 80** on a TCL NXTPAPER. |
| **Disable animated menu background** | Drops the 3D shell map, the biggest menu cost on weak GPUs. |
| **Windowed** | Run windowed rather than fullscreen. |
| **Storage info** | Where data lives, free space, what's installed, and the `adb push` command if you prefer the PC route. |

The launcher shows the exact engine flags it will pass, so what it does is never
a mystery.

### Multiple installs and mods

Data sets live side by side, so a mod never overwrites your base game:

```
Android/data/me.generalsx.zh/files/
  GameData/                  <- your main install (unchanged)
  Profiles/ShockWave/        <- additional full data sets
  Profiles/RiseOfTheReds/
  Mods/<name>/               <- partial mods, layered with -mod
```

Importing when data already exists asks whether to add it as a new profile or
replace the existing one.

Total conversions such as [ShockWave and Rise of the
Reds](https://www.moddb.com/games/cc-generals-zero-hour) ship as a full data set
(import as a **profile**) or as files layered over Zero Hour (import as a
**mod**). Both routes are supported. The mod plumbing uses the engine's own
`-mod` flag; specific mods have not been individually tested.

### Under the hood

The launcher passes ordinary engine command-line flags — `-nologo`,
`-noshellmap`, `-win`, `-mod <path>` — all of which the 2003 engine already
understood. Only one addition was needed: **`-datadir <path>`**, handled in
`SDL3Main.cpp` before the working directory is set, which is what makes
switching between installs possible.

Start the game through the launcher. Since v0.12, GameActivity is private to the app.

### Playing the original Generals

Pick **Command & Conquer: Generals (original)** in the launcher's Game dropdown.
It runs the separate `libmain_generals.so` engine, and remembers its own data
selection, so switching games does not disturb your Zero Hour setup.

> **Generals needs its own data folder.** A Complete Edition install puts *both*
> games' archives in one directory (`INI.big` **and** `INIZH.big`, `Audio.big`
> **and** `AudioZH.big`, …). The engine loads every `.big` in its working
> directory, so pointing Generals at that folder feeds it Zero Hour's archives
> and it fails parsing `Multiplayer.ini` — the two games' INI schemas differ.
>
> Import your Generals files as their own **profile**, or copy just the 15
> base-game archives (everything *without* `ZH` in the name):
>
> ```
> Audio.big  AudioEnglish.big  English.big  INI.big  Music.big  Patch.big
> Speech.big  SpeechEnglish.big  Terrain.big  Textures.big  W3D.big
> Window.big  gensec.big  maps.big  shaders.big
> ```
>
> Prefer the launcher importer; access to shell-copied files varies with Android storage restrictions.

---

## Touch Controls

The 2003 engine expects a mouse with two buttons, a wheel, and a cursor that
exists even when nothing is pressed. Mapping that onto a touchscreen naively
makes an RTS miserable, so the port uses a gesture scheme built around *"the map
lives under your finger"*:

| Gesture | Action |
|---------|--------|
| **Tap** | Left-click — order, rally point, button |
| **Drag one finger** | Pan the camera; the map tracks your finger 1:1 |
| **Double tap on a unit** | Select every unit of that type on screen |
| **Double tap on the ground** | Deselect (right-click) |
| **Double tap, then hold and drag** | Selection box — the first click is swallowed, so you never issue a stray order |
| **Long press, finger still** | Right-click |
| **Two-finger drag** | Pan; **pinch** zooms 1:1 through the actual camera height |
| **Drag over the UI** | Ordinary left-drag, for scrollbars and lists |
| **While placing a building** | One finger positions it, a second finger rotates it toward that point; lift both to place |
| **Tap during a movie** | Skip the intro or a cutscene |

Taps fire immediately rather than waiting out the double-tap window, so there is
no input lag on orders; the double tap is detected on top of the click that has
already been sent, exactly as on desktop.

> The gesture scheme is ported from
> [wingear's GLES fork](https://github.com/wingear/GeneralsZH-Android-OpenGL-ES)
> (GPL-3.0). The design credit is theirs.

## Mouse Support

A USB or Bluetooth mouse works alongside touch — movement, left and right click,
camera drag, and edge-of-screen scrolling. Edge scrolling activates only when a
real mouse is present, because a touch cursor stays wherever you last tapped and
would otherwise pin the camera scrolling at the screen edge.

Android input has several traps here that are not obvious (a mouse also emits
touch events; `SDL_HasMouse()` reports false with a mouse attached; right-click
arrives as the BACK button). If you are working on input, read
[`docs/port/ANDROID_INPUT.md`](docs/port/ANDROID_INPUT.md) first.

---

## Build from Source

See [Android build and packaging](scripts/build/android/README.md) for the
current tool versions and the verified-runtime packaging path. A default native
build can still stage the retired DXVK 2.6 runtime; a successful Gradle build
alone does not establish release readiness.

### Unified release renderer

The unified release renderer uses two pinned upstream components:

- `Joshua-Ashton/dxvk-native`, tag `native-1.9.2b`, commit
  `c8dc91fabd00cac11d697ccf07426e798393cd40`
- `crosire/d3d8to9`, commit
  `6cdb8a82184898f1b9371e4c8412c2d33ebb7b51`

Apply [`Patches/dxvk-native-1.9.2b-android-sdl3.patch`](Patches/dxvk-native-1.9.2b-android-sdl3.patch)
and [`Patches/d3d8to9-android-arm64.patch`](Patches/d3d8to9-android-arm64.patch)
to those exact revisions. The `android/mali-spike` module provides the D3D9 and
D3D8 hardware gates used before packaging the full app.

Mali GPUs do not expose the game's BC/DXT texture formats. The renderer reports
those formats unavailable, allowing the existing engine path to decode DXT1,
DXT3, and DXT5 textures to uploadable RGBA surfaces. No game assets are modified.

For more details, see [`android.md`](android.md) — the complete engineering log
of every bug found and fixed during the port.

---

## What This Port Involved

Getting a 2003 Windows DirectX 8 game running natively on Android required:

1. **DXVK for Android aarch64** — DXVK had never been built for Android. Required
   gating x86 SSE flags, fixing the SDL3 WSI soname, and a high-DPI WSI patch.
2. **Android NDK cross-compilation** of the 500k LOC engine — resolving every
   missing POSIX function (`pthread_cancel`, `glob`, `sys/timeb`), every libc++
   difference (`std::from_chars` float overload missing), and every assumption
   the engine made about having a writable filesystem.
3. **BIG archive file override** — the engine's archive system loaded base
   Generals data instead of Zero Hour data because of a `multimap::insert` hint
   ordering bug. Fixed with erase-and-reinsert to guarantee override precedence.
4. **Memory allocator coexistence** — the engine's custom DMA allocator
   intercepted all `operator delete` calls, including those from OpenAL and
   libc++. Fixed with a magic cookie check to distinguish engine allocations.
5. **DXVK device creation** — the `CreateDevice` call failed because the engine's
   fullscreen format-selection path returned `D3DFMT_UNKNOWN` on Android (no
   desktop display modes). Fixed by forcing the windowed format path on all
   non-Windows platforms.
6. **Font extraction** — Android APK assets are invisible to `fopen()`. The
   engine's FreeType font locator expects `fonts/*.ttf` on the filesystem.
   Added JNI-based extraction from `AAssetManager` on first launch.

**Full bug list with root causes and fixes:** [`android.md`](android.md)

---

## Documentation

| Document | What it covers |
|---|---|
| [`docs/port/ANDROID_INPUT.md`](docs/port/ANDROID_INPUT.md) | The touch gesture scheme, and the four Android mouse traps that caused regressions (mice emit touch events; `SDL_HasMouse()` lies; a real mouse reports device id `0`; right-click arrives as BACK). **Read before touching input.** |
| [`docs/port/ANDROID_FFMPEG.md`](docs/port/ANDROID_FFMPEG.md) | The two independent FFmpeg stub layers, why a stubbed build reports SUCCESS with no audio or video, and the one-command check that detects it. Also the Bink `bink` vs `binkvideo` configure trap and the soname fix. |
| [`docs/port/KNOWN_ISSUE_BLACK_MODELS.md`](docs/port/KNOWN_ISSUE_BLACK_MODELS.md) | Historical DXT3 black-building investigation, fixed in v0.10; preserves the evidence and rejected theories. |
| [`docs/port/PORTING_PLAYBOOK.md`](docs/port/PORTING_PLAYBOOK.md) | General porting workflow |
| [`docs/port/PORTING_PATTERNS.md`](docs/port/PORTING_PATTERNS.md) | Recurring code patterns used across the port |
| [`docs/port/RELEASE_CHECKLIST.md`](docs/port/RELEASE_CHECKLIST.md) | Steps before cutting a release |
| [`android.md`](android.md) | Full engineering log of every bug found and fixed |

---

## Architecture

```
┌──────────────────────────────────────────────┐
│              Game Engine (C++)                │
│         (~500k LOC, GPL v3 source)            │
├──────────────────────────────────────────────┤
│  Graphics: DirectX 8 API calls                │
│     ↓                                         │
│  DXVK (libdxvk_d3d8.so) — D3D8 → Vulkan      │
│     ↓                                         │
│  Android Vulkan Driver (Adreno / Mali)        │
├──────────────────────────────────────────────┤
│  Windowing: SDL3 (touch → synthetic mouse)    │
│  Audio: OpenAL Soft + FFmpeg (Oboe backend)   │
│  Video: FFmpeg 8.1.1 with Bink decoders       │
├──────────────────────────────────────────────┤
│  Android OS (arm64-v8a, API 24+)              │
└──────────────────────────────────────────────┘
```

The engine speaks DirectX 8. DXVK translates those calls to Vulkan. The Android
Vulkan driver renders to the screen. No Wine, no QEMU, no emulation — the engine
itself is compiled as a native Android shared library (`libmain.so`).

---

## Lineage & Credits

This port stands on a chain of community work:

- **Westwood / EA Pacific** — the original game
- **EA** — the GPL v3 source release
- **[TheSuperHackers](https://github.com/TheSuperHackers/GeneralsGameCode)** — community mainline: build modernization, VC6→modern toolchain, cross-platform groundwork
- **[Fighter19](https://github.com/Fighter19/CnC_Generals_Zero_Hour)** — original Unix/64-bit port: SDL3, DXVK approach, FreeType text rendering
- **[fbraz3/GeneralsX](https://github.com/fbraz3/GeneralsX)** — macOS/Linux port integrating the above
- **[ammaarreshi/Generals-Mac-iOS-iPad](https://github.com/ammaarreshi/Generals-Mac-iOS-iPad)** — iOS/iPadOS port (DXVK-on-iOS, touch controls, app lifecycle)
- **[wingear/GeneralsZH-Android-OpenGL-ES](https://github.com/wingear/GeneralsZH-Android-OpenGL-ES)** — **the RTS touch control scheme used by this port** (instant-tap clicks with double-tap on top, 1:1 map panning, pinch zoom anchored to real camera height, two-finger building placement), plus the OpenAL buffer-depth fix for underrun clicking. Their fork also independently found the FFmpeg stub trap. A hand-written D3D8→OpenGL ES 3.0 backend for devices where DXVK cannot create a device at all — worth reading if your GPU is below the DXVK bar.
- **This fork** — the Android arm64 port
- **DXVK, SDL3, OpenAL Soft, Liberation Fonts** — the open-source load-bearing walls

Engine code is **GPL v3** (EA's source release → the chain above → this fork).
Game assets are not included, not licensed here, and not distributed.

---

## Legal

This project does not include or distribute any game assets, data files, or
 copyrighted game content. It is an engine port that requires the user to
 provide their own legally-obtained copy of Command & Conquer Generals Zero Hour.

Command & Conquer is a trademark of Electronic Arts Inc. This project is not
affiliated with or endorsed by Electronic Arts.
