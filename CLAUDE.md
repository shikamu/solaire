# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

Solaire is a 3D space-shooter with single-player, LAN-server, and LAN-client modes. It's an older (circa-2016) C++ hobby project being revived. Engine: **Irrlicht 1.7.3**; audio: **FMOD Ex**; rendering shaders are **HLSL/Direct3D 9**. All source lives in `Solaire/` (87 `.cpp` files, flat directory).

## Build & run

- Open `Solaire.sln` in Visual Studio. **Win32 only** — configurations are `Debug|Win32` and `Release|Win32` (there is no x64 config). Build → Rebuild Solution, then F5.
- **External dependencies are referenced by absolute path** in `Solaire/Solaire.vcxproj`:
  - Irrlicht 1.7.3 at `C:\irrlicht-1.7.3` (with `G:\irrlicht-1.7.3` as a fallback in `IncludePath`/`LibraryPath`). Links `Irrlicht.lib` and `Ws2_32.lib` via `#pragma comment(lib, ...)` in `System.cpp`.
  - FMOD under `..\fmod` (`fmodex_vc.lib`, with `fmodex.dll` checked in beside the project).
- **Runtime DLL/asset resolution** (this is the usual breakage point):
  - A post-build event copies `Irrlicht.dll` from the Irrlicht `bin\Win32-VisualStudio` folder into `$(OutDir)` (`Debug\` / `Release\`) so the exe runs both from VS and standalone.
  - `Solaire.vcxproj.user` adds the Irrlicht bin to the debugger `PATH` for F5 launches.
  - The debugger working directory defaults to the **project dir** `Solaire\`, which is where `data/` (textures, `.hlsl` shaders, models, fonts, `.mp3`) and `fmodex.dll` live. Assets are loaded by paths relative to that directory, so the game must run with that working dir.
- Console subsystem; a fresh `Output.txt` log is truncated at startup in `main()`. There is no test suite, linter, or CLI build script — Visual Studio is the only build path.

## Architecture

### Central hub and main loop
`System` (`System.h/.cpp`) is the root singleton (`System::get()`). `main()` calls `System::run()`, which drives the Irrlicht device loop. Each frame, under `m_sceneLock`: render via `RenderManager::get().render()`, then `m_currentScene->update(dt)`. `System` owns one instance of every scene and swaps the active one via `toggle(SCENE_MENU|SCENE_SINGLE|SCENE_LAN_CLIENT|SCENE_LAN_SERVER)`; scene init is deferred (`m_pendingInit`) so a loading screen can render first.

### Scenes
`LogicScene` is the base; subclasses are `MenuScene`, `SinglePlayerScene`, `LANClientScene`, `LANServerScene`. A scene owns the authoritative `map<unsigned int, SpaceObject*>` (keyed by global ID), the player's `Agent`, a `SpawnQueue`, explosions, and the scoreboard. Object IDs are pooled (`RequestSpaceObjectID`/`ReleaseID`). Per-scene GUI events go through a matching `*GUIEventListener`.

### Game-object model — `SpaceObject`
`SpaceObject` is the core entity and acts as a **pointer hub** wiring together the subsystems for one ship/projectile:
- `RenderObject` — visuals (wraps an Irrlicht scene node; styled by `Shader` subclasses).
- `LockPointer<PhysicsObject>` — physics body, simulated on the physics thread.
- `Actuator` — converts control intent into movement.
- `TargettingModule` + soft/hard target pointers.
- `vector<ModuleNode*> Modules` — fitted equipment/weapons.
- Dynamic state: `map<PropertyTypeID, PropertyState>`, position/rotation/velocity, shield/armour.
- Identity: global `ID` + `ObjectMask` (team/faction bitmask used for collision and friend/foe checks).

### Control abstraction — Agent / Actuator
This is the key indirection that lets the *same* `SpaceObject` be driven by a local human, AI, or the network:
- **`Agent`** = the controller of a ship plus its HUD (reticule, target info, shield/armour bars). Subclasses: `LocalAgent`, `LocalClientAgent`, `RemoteAgent`, `ServerLocalAgent`, `DummyAgent`.
- **`Actuator`** = produces per-frame control (`ActuatorData`: pitch/yaw/roll/thrust + a button mask, convertible to/from `ActuatorOutput` for the wire). Subclasses: `HumanActuator`/`ClientHumanActuator` (input), `AIActuator`, `NetworkActuator`, `DummyActuator`, `WarheadActuator`. Actuators support hot-swap via `GetReplacement()`/`NeedsReplacement()`.

### Modules (weapons/systems)
A `ModuleNode` is a hardpoint on a `SpaceObject` holding one `LogicModule`. `LogicModule` subclasses (e.g. `MachineGunModule`, `RocketLauncherModule`, `TorpedoLauncherModule`, `ShieldRegenModule`, `TargettingModule`, defined in `ModuleTemplates.h` and individual `.cpp`s) implement `Activate()`/`Update(dt)` and have cooldowns. They are triggered by the actuator's button mask via an `ActivationType`.

### Threading model
`Task` (`Task.h`) is the thread base class (Win32 `_beginthreadex`, `__stdcall run()`). Long-running subsystems run as their own threads/Tasks: `PhysicsController`, `GarbageCollector`, `AudioManager`, and the networking `LANServer`/`LANClient`. Cross-thread safety uses:
- **`CSLock`** — a Win32 critical-section wrapper, used pervasively (scene lock, list locks, module-list locks).
- **`LockPointer<T>`** — wraps objects shared between the logic and physics threads (`PhysicsObject`, `SpaceObjectShell`) for guarded access.
When touching `SpaceObject`/scene state, respect the existing `AcquireLock`/`ReleaseLock` discipline — the physics thread and logic thread both reach into these maps.

### Networking (TCP LAN)
`NetworkController::get()` is the hub holding the active `LANServer`, `LANClient`, `LANServerScene`, and `LANFinalView`, and does `WSAStartup` (Winsock, `Ws2_32`). The server is authoritative: clients send `ActuatorData`; the server simulates and broadcasts `SpaceObjectNetworkInfo` (position/rotation/shield/armour/target), which `System::updateSpaceObject()` applies to the matching local `SpaceObject` by ID. Packets are `TCPPacket`s built by `TCPPacketFactory`. LAN game discovery uses `GameAdvertiser`/`GameListener`.

### Subsystem singletons
Most managers are static-instance singletons accessed via `::get()` / `::Get()`: `System`, `RenderManager` (+`AutoCamera`), `PhysicsController`, `SpaceObjectFactory`, `AudioManager` (FMOD), `GarbageCollector`, `NetworkController`, `TCPPacketFactory`, `NetworkUtilities`, `GameLog`. New `SpaceObject`s are assembled through `SpaceObjectFactory`.

### Rendering & input
`Shader` subclasses (`ShipShader`, `HUDShader`, `ProjectileShader`, `ShieldShader`, `WarheadShader`, etc.) bind the HLSL files in `data/`. `ConfigData` owns Irrlicht device creation, the `EventReceiver`, and `KeyBindings` (loaded from joystick/keybinding config files such as `JoystickConfigXBOX.txt`); keybindings are re-registered whenever a gameplay scene becomes active.

## Conventions & gotchas

- Manual memory management throughout (raw `new`/`delete`, owning raw pointers); the Debug build enables CRT leak checking via `_CrtSetDbgFlag` in `System.cpp`.
- Ownership comments matter: e.g. a `ModuleNode` deletes its `FittedModule` unless it was transferred; `SpaceObject` deletes its `TargettingModule`. Check the header comments before changing lifetimes.
- There is a lot of commented-out / experimental code and a few self-described "hack" methods (e.g. `System::feedDataToLANCLient`) — expected for a revived prototype; don't treat them as dead-code cleanup targets without checking call sites.
- Many files include `<irrlicht.h>` / `<Windows.h>`; `Task.h` defines `WIN32_LEAN_AND_MEAN` and `NOMINMAX` before `<Windows.h>` — preserve that ordering when adding Windows includes.
