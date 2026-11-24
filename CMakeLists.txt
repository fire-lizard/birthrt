# CMakeList.txt: проект CMake для Birthrt.
#
cmake_minimum_required (VERSION 3.10)
project (Birthrt CXX C)

# Portable core - builds on Windows, Linux and macOS.
set (BIRTHRT_CORE
	"2DMOUSE.C" "2DMOUSE.H" "ABILITY.CPP" "ABILITY.HXX"
	"ACTIONS.CPP" "ACTIONS.HXX" "ACTNMENU.CPP" "ACTNMENU.HXX"
	"ADVENTUR.CPP" "ADVENTUR.HXX" "ADVPREP.CPP" "ADVPREP.HXX"
	"AIBTLCAP.CPP" "AIBTLCAP.HXX" "AIFIREBL.CPP" "AIFIREBL.HXX"
	"AIFOLCAP.HXX" "AIFOLID.HXX" "AIFOLPLY.CPP"
	"AIFOLPLY.HXX" "AIFOLPTH.CPP" "AIFOLPTH.HXX" "AIFTSEQ.CPP"
	"AIFTSEQ.HXX" "AIGARGL.CPP" "AIGARGL.HXX" "AIGUARD.HXX"
	"AIHARPY.CPP" "AIHARPY.HXX" "AIHHOUND.CPP" "AIHHOUND.HXX"
	"AIHORSE.CPP" "AIHORSE.HXX" "AILEADER.HXX"
	"AIPATHPT.HXX" "AIPONG.CPP" "AIPONG.HXX"
	"AISPECTR.CPP" "AISPECTR.HXX" "AISPIDER.CPP" "AISPIDER.HXX"
	"AISTUN.CPP" "AISTUN.HXX" "AIWYVERN.CPP" "AIWYVERN.HXX"
	"ALIGNMNT.CPP" "ALIGNMNT.HXX" "AMBIENCE.INC" "ANIM.CPP"
	"ANIM.H" "APPEAR.CPP" "APPEAR.HXX" "ARMOR.CPP"
	"ARMOR.HXX" "ASSERT.C" "ATHING.CPP"
	"ATHING.HXX" "AVATAR.CPP"
	"AVATAR.HXX" "BATTLE.CPP" "BATTLE.HXX" "BATTLEUI.CPP"
	"BATTLEUI.HXX" "BLOCKM.C" "BLOOD.CPP" "BLOOD.HXX"
	"BLOODEFF.CPP" "BLOODREQ.CPP" "BLOODREQ.HXX" "BUMP.C"
	"BUMP.H" "CAMERA.C" "CAMERA.H" "CAMRDATA.HXX"
	"CHARISMA.CPP" "CHARISMA.HXX" "CHARSEL.CPP" "CHARSEL.HXX"
	"CLIP.C" "COLORS.H" "COMBATUI.CPP" "COMBATUI.HXX"
	"COMBCAMR.CPP" "COMBCAMR.HXX" "COMBCNTL.CPP" "COMBCNTL.HXX"
	"COMBDATA.CPP" "COMBDATA.HXX" "COMBOPTS.CPP" "COMBOPTS.HXX"
	"COMBTARG.CPP" "COMBTARG.HXX" "CONSTITN.CPP" "CONSTITN.HXX"
	"CONTEXT.HXX" "CONVERT.C" "CTHING.HXX" "CUE.C"
	"DEBUG.C" "DEBUG.H" "DESCRIBE.CPP"
	"DESCRIBE.HXX" "DEXTERIT.CPP" "DEXTERIT.HXX"
	"DICE.C" "DICE.H" "DOORS.C" "DOORS.H"
	"DOSHELP.CPP" "DOSHELP.HXX" "DYNAMTEX.C" "DYNAMTEX.H"
	"EFFSEPAR.HXX" "ENGINE.H" "ENGINT.H" "EXPERIEN.CPP"
	"EXPERIEN.HXX" "EXPLEVEL.CPP" "EXPLEVEL.HXX" "EXTENT.C"
	"FILENAME.HXX" "FILEUTIL.C" "FILEUTIL.H" "FINDFILE.CPP"
	"FINDFILE.HXX" "FLICPLAY.CPP" "FLICPLAY.HXX" "FOPEN.C"
	"FOPEN.H" "GAME.CPP" "GAME.H" "GAMECAMR.HXX"
	"GAMEKEY.CPP" "GAMEKEY.HXX" "GAMEMAP.CPP" "GAMEMAP.HXX"
	"GAMEMENU.H" "GAMEOPT.CPP" "GAMESTR.H" "GAMETYPE.CPP"
	"GAMETYPE.HXX" "GMENUENM.H" "HANDLE.HXX" "HMGR.CPP"
	"HMGR.HXX" "INFOBOX.CPP" "INFOBOX.HXX" "INTELLI.CPP"
	"INTELLI.HXX" "INVNBUTS.HXX" "INVNGUI.CPP" "INVNGUI.HXX"
	"INVNTORY.CPP" "INVNTORY.HXX" "ITEMEFF.CPP" "ITEMEQP.CPP"
	"ITEMEQP.HXX" "ITEMTYPE.CPP" "ITEMTYPE.HXX" "ITEMUTIL.CPP"
	"ITEMUTIL.HXX" "LEVEL.C" "LIGHT.H"
	"LOADSAVE.CPP" "LOADSAVE.HXX" "MACHINE.H"
	"MACHINT.H" "MAGIC.C" "MAGIC.H" "MAIN.CPP"
	"MAIN.HXX" "MAP.C" "MAP.H" "MAPAI.CPP"
	"MAPAI.HXX" "MAPAITBL.CPP" "MAPEFF.CPP" "MAPICON.HXX"
	"MARGIN.CPP" "MARGIN.HXX" "MEMMANAG.CPP" "MEMMANAG.HXX"
	"MENU.C" "MENU.H" "MENUDATA.H" "MENUEFF.CPP"
	"MENUPROT.H" "MULTIMAP.CPP" "MULTIMAP.HXX" "MULTIUI.CPP"
	"MULTIUI.HXX" "PANEL.C" "PANEL.H" "PANORAMA.CPP"
	"PANORAMA.HXX" "PARCH.CPP" "PARCH.HXX" "PLACES.CPP"
	"PLACES.HXX" "PLAYCLAS.CPP" "PLAYCLAS.HXX" "PLAYER.CPP"
	"PLAYER.HXX" "PLAYHIT.CPP" "PLAYHIT.HXX" "PLAYID.CPP"
	"PLAYID.HXX" "PLAYRACE.CPP" "PLAYRACE.HXX" "PLAYSTAT.CPP"
	"PLAYSTAT.HXX" "POINT.C" "PRACTICE.CPP"
	"PROVDATA.CPP" "PROVDATA.HXX" "PROVTAX.C"
	"PROVTAX.H" "RANDOM.C" "RANDOM.H" "REALM.CPP"
	"REALM.HXX" "REDBOOK.C" "REDBOOK.H" "REGEN.CPP"
	"REGEN.HXX" "REGENTS.CPP" "REGENTS.HXX" "REGIONS.C"
	"REGIONS.H" "REGRELIC.CPP" "REGRELIC.HXX" "REGUTIL.CPP"
	"REJECT.C" "RELICEFF.CPP" "RENDER.C" "REPORT.CPP"
	"REPORT.HXX" "REQUEST.C" "REQUEST.H" "RESMANAG.CPP"
	"RESMANAG.HXX" "RESOURCE.H" "RESURECT.HXX" "RLMSPELL.CPP"
	"RLMSPELL.HXX" "SCAN.C" "SCENE.CPP" "SCENE.HXX"
	"SCNAI.CPP" "SCNAI.HXX" "SCNCAMR.CPP" "SCNCAMR.HXX"
	"SCNDEMO.HXX" "SCNDLGS.HXX" "SCNEXIT.HXX" "SCNINFO.HXX"
	"SCNKEY.CPP" "SCNKEY.HXX" "SCNMAP.CPP" "SCNMAP.HXX"
	"SCNMGR.CPP" "SCNMGR.HXX" "SCNMUSIC.HXX" "SCNNOTES.HXX"
	"SCNPLACE.HXX" "SCNSCTOR.HXX" "SCNVER.CPP" "SCNVER.HXX"
	"SECTORS.C" "SECTORS.H" "SINCOS.H"
	"SNDVOX.CPP" "SNDVOX.HXX"
	"SOUND.CPP" "SOUND.HXX" "SOUNDFX.HXX" "SPANS.C"
	"SPECIAL.H" "SPELL.CPP" "SPELL.HXX" "SPELLEFF.CPP"
	"SPELLPRO.CPP" "SPELLPRO.HXX" "STATSMOD.HXX" "STRENGTH.CPP"
	"STRENGTH.HXX" "strenum.h" "STRINT.H"
	"STRMGR.C" "STRMGR.H" "SYSINT.H" "SYSTEM.C"
	"SYSTEM.H" "TASK.C" "TASK.H" "TEXTURES.C"
	"THINGS.C" "THINGS.H" "THINGTYP.H" "TIMER.C"
	"TMSGBOX.CPP" "TMSGBOX.HXX" "TUTOR.CPP"
	"TUTOR.HXX" "TUTRMENU.CPP" "TUTRMENU.HXX" "TYPEDEFS.H"
	"UNITS.CPP" "UNITS.HXX" "VECTOR.HXX"
	"VERSION.CPP" "VERSION.HXX" "WADTHING.H" "WEAPON.CPP"
	"WEAPON.HXX" "WISDOM.CPP" "WISDOM.HXX" "PLATFORM.H"
	"PLATFORM.CPP")

# Windows-only: DirectDraw/DirectSound/DirectPlay and the Win32 message
# loop. The POSIX build leaves these out; the
# SDL backend that replaces them is not written yet.
set (BIRTHRT_WIN32
	"WINSYS/CRC.CPP" "WINSYS/DDRAW.CPP" "WINSYS/DDRAWPRO.H"
	"WINSYS/FILEXFER.CPP" "WINSYS/FILEXFER.HXX" "WINSYS/GMULPLAY.CPP" "WINSYS/GMULPLAY.HXX"
	"WINSYS/GOTOURL.CPP" "WINSYS/GOTOURL.H" "WINSYS/KEYINT.C"
	"WINSYS/MACHINE.C"
	"WINSYS/MESSQUE.CPP" "WINSYS/MESSQUE.HXX" "WINSYS/MONO.CPP"
	"WINSYS/MONO.H" "WINSYS/MONO_C.CPP" "WINSYS/MONO_C.H" "WINSYS/MOUSE.C"
	"WINSYS/MULPLAY.CPP" "WINSYS/MULPLAY.HXX"
	"WINSYS/PNETMOD.CPP" "WINSYS/PNETMOD.HXX" "WINSYS/PNETTYPE.H"
	"WINSYS/SIERRANW.H"
	"WINSYS/SNTYPES.H"
	"WINSYS/SNWVALID.H" "WINSYS/STATUS.C" "WINSYS/STATUS.H"
	"WINSYS/SYSGRAPH.CPP" "WINSYS/UTIL.C"
	"KERNAUD.CPP" "KERNAUD.HPP"
	"SOLAUDIO/Dsound.cpp" "SOLAUDIO/DSOUND.HPP"
	"BIRTHRT.RC")

if (WIN32)

	add_executable (Birthrt WIN32 ${BIRTHRT_CORE} ${BIRTHRT_WIN32})

	include_directories("C:\\Program Files (x86)\\Microsoft DirectX 9.0 SDK (April 2005)\\Include"
	"C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.26100.0\\um")
	target_link_libraries(Birthrt "C:\\Program Files (x86)\\Microsoft DirectX 9.0 SDK (April 2005)\\Lib\\x64\\ddraw.lib"
	"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.26100.0\\um\\x64\\winmm.lib")
	# apBlocks[] now stores heap offsets rather than truncated pointers, so
	# this is no longer load-bearing - kept until the resource manager has
	# had the same audit.
	target_link_options(Birthrt PRIVATE "/LARGEADDRESSAWARE:NO")
	# Audio: SOLAUDIO DirectSound backend + reconstructed KERNAUD kernel glue.
	target_compile_definitions(Birthrt PRIVATE AUDIO_LIB)
	# Debug info in optimized builds too - crash dumps stay symbolizable.
	target_compile_options(Birthrt PRIVATE "$<$<CONFIG:Release>:/Zi>")
	target_link_options(Birthrt PRIVATE "$<$<CONFIG:Release>:/DEBUG>")

else ()

	# SDL2 replaces WINSYS: window, present, palette, keyboard, mouse.
	find_package (SDL2 REQUIRED)

	# POSIX/ is the Linux/macOS counterpart of WINSYS/.
	# MESSQUE.CPP is in the WINSYS directory but is portable - a plain
	# message ring the battle AI uses, no Win32 in it.
	set (BIRTHRT_POSIX
		"POSIX/SDLSYS.CPP" "POSIX/STUBS.CPP" "POSIX/MULPLAY_STUB.CPP"
		"WINSYS/MESSQUE.CPP"
		# Audio: KERNAUD.CPP is the portable half (wave lookup, slot table,
		# RIFF parsing) and is shared with the Win32 build; SDLAUDIO.CPP
		# replaces SOLAUDIO's DirectSound backend under it.
		"KERNAUD.CPP" "KERNAUD.HPP" "POSIX/SDLAUDIO.CPP" "POSIX/SDLAUDIO.HPP")

	add_executable (Birthrt ${BIRTHRT_CORE} ${BIRTHRT_POSIX})

	# The 1997 sources are C++ even in the .C files - the Win32 build
	# compiles them that way, so cross-TU globals are C++-mangled there.
	# Pin the same choice here instead of letting the extension decide.
	# Only the .C/.CPP entries: setting LANGUAGE on the .H/.HXX entries
	# promotes them from headers to translation units and CMake compiles
	# each one standalone.
	foreach (_src IN LISTS BIRTHRT_CORE BIRTHRT_POSIX)
		if (_src MATCHES "\\.(C|CPP|c|cpp)$")
			set_source_files_properties (${_src} PROPERTIES LANGUAGE CXX)
		endif ()
	endforeach ()

	# _WINDOWS is this codebase's own "not DOS" switch (595 conditionals).
	# The non-DOS branch is the right one on Linux/macOS too.
	# AUDIO_LIB switches SOUND.CPP onto the kernel->Audio* path.
	target_compile_definitions (Birthrt PRIVATE _WINDOWS AUDIO_LIB)
	target_include_directories (Birthrt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${SDL2_INCLUDE_DIRS})
	# -rdynamic exports symbols into the dynamic table so the crash
	# handler's backtrace shows function names rather than bare addresses.
	# clang's Darwin driver translates it to ld64's -export_dynamic, so it
	# is meaningful on macOS too.
	#
	# -no-pie was the Linux counterpart of the Windows build's
	# /LARGEADDRESSAWARE:NO: the region/menu system used to hand callbacks
	# their argument as a 32-bit LONG while ~14 sites pushed a pointer
	# through it, and loading the image at 0x400000 kept those addresses
	# inside 32 bits. That deferral has now been cashed in - the argument is
	# REGARG (intptr_t, TYPEDEFS.H) and the round trip is lossless
	# everywhere - so -no-pie is no longer load-bearing. Kept on Linux only
	# because the existing build is known-good with it.
	#
	# It could not have been carried to macOS in any case: clang forces PIC
	# on both Darwin arches, ld64 ignores -no_pie on arm64, and every 64-bit
	# Mach-O reserves a 4GB __PAGEZERO, so __DATA starts at 0x100000000 and
	# no macOS global has ever fit in a LONG.
	target_link_libraries (Birthrt ${SDL2_LIBRARIES})
	if (APPLE)
		target_link_options (Birthrt PRIVATE -rdynamic)
	else ()
		target_link_options (Birthrt PRIVATE -rdynamic -no-pie)
		target_compile_options (Birthrt PRIVATE -fno-pie)
	endif ()
	set_target_properties (Birthrt PROPERTIES CXX_STANDARD 11)

	# 1997 C++ predates most of what these would flag, and the noise buries
	# the diagnostics that matter. -fpermissive is load-bearing on GCC: the
	# sources rely on pre-standard conversions it would otherwise reject.
	# It is GCC-only - clang parses it into its ignored-flags group and
	# silently does nothing - so gate it rather than imply it does something
	# on macOS. Anything clang rejects has to be fixed at the source; there
	# is no permissive mode to fall back on.
	# -Werror=overflow: the one warning class this port cannot afford to let
	# scroll past. `LONG x = LONG_MAX` silently became -1 here and left the
	# adventure monsters unable to pick a target; 19 sites had it. It is a
	# constant-folding diagnostic, so it never fires on correct code.
	# Note clang's "overflow" group is an empty GCC-compatibility stub, so
	# this promotes nothing there; the clang spelling is
	# -Werror=constant-conversion, not turned on until the macOS build is
	# green and the existing hits have been counted.
	target_compile_options (Birthrt PRIVATE
		$<$<CXX_COMPILER_ID:GNU>:-fpermissive>
		-fsigned-char -fno-strict-aliasing
		-Werror=overflow
		-Wno-write-strings -Wno-narrowing -Wno-multichar
		-Wno-invalid-offsetof -Wno-deprecated)

	# REPORT.CPP's Subject table stores "<advisor group> + index" in an 8-bit
	# bitfield and some entries exceed 255. MSVC truncates them identically,
	# so this matches the retail build - it is not a porting artefact, and
	# widening the field would move reports between advisor tabs.
	set_source_files_properties (REPORT.CPP PROPERTIES COMPILE_OPTIONS "-Wno-error=overflow")

	message (STATUS "Birthrt: POSIX/SDL2 build.")
	message (STATUS "  Not yet implemented: DirectPlay multiplayer.")

endif ()

# Standalone string-table disassembler: strdat.dat -> strdat.txt + strenum.h,
# the inverse of STRMAKE.CPP. Its own target because it owns main().
# PLATFORM.CPP because STRUMAKE.CPP includes PLATFORM.H, whose non-Win32 arm
# rewrites its four fopen() calls to plat_fopen(). Without it the target does
# not link off Windows - never noticed because EXCLUDE_FROM_ALL means a plain
# `cmake --build` never touches it.
add_executable (strumake EXCLUDE_FROM_ALL "STRUMAKE.CPP" "PLATFORM.CPP")
target_include_directories (strumake PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
if (MSVC)
	target_compile_definitions (strumake PRIVATE _CRT_SECURE_NO_WARNINGS)
endif ()

# String-table compiler: strdat.txt -> strdat.dat + strenum.h. Same shape
# as strumake above, for the same reasons.
add_executable (strmake EXCLUDE_FROM_ALL "STRMAKE.CPP" "PLATFORM.CPP")
target_include_directories (strmake PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
if (MSVC)
	target_compile_definitions (strmake PRIVATE _CRT_SECURE_NO_WARNINGS)
endif ()

# Birthright I Character Editor (1997, Windows only) - source kept for a
# possible future tool; no target builds it. It was a separate program built
# from the game sources with _CHARED defined: MAINCHED.CPP and
# WINSYS/MACHINEX.C stand in for MAIN.CPP and WINSYS/MACHINE.C, and
# CHARED.CPP, CHAREDIN.CPP, AVA.CPP and GETFILE.CPP are the editor itself.
# The game modules it needs (ability and bloodline rolls, name tables) keep
# that code under #if defined(_CHARED). Its dialog resources (Chared.rc) were
# never in this repository - RESOURCE.H and CHAREDSY.H hold the control ids -
# so the dialogs would have to be recreated before it can run.

# Birthright I level and scene editors (1996-97, Windows only). Both are the
# game itself built with extra defines and sources, so these options turn the
# Birthrt target into an editor - give them their own build directory:
#   BIRTHRT_LEVEL_EDITOR  _EDIT ("WAD Builders Edition"): adds EDIT.C (walls,
#                         textures, lighting) and SAVEPWAD.C (saves the level
#                         as a PWAD).
#   BIRTHRT_WADBUILDERS   _WADBUILDERS: a few designer conveniences (walk
#                         through walls, quit without confirming).
#   BIRTHRT_SCENE_EDITOR  SCENE_EDITOR: adds DLG_EDIT.CPP (place characters,
#                         edit their AI); saves the .scn file via
#                         SCENE::mfWriteSceneFile.
# All off by default; the hooks in the game sources sit under those #if's, so
# the normal game never compiles them. The options combine and every
# combination compiles and links. Their dialog resources were never in this
# repository, so the dialogs would have to be recreated before either editor
# can run.
option (BIRTHRT_LEVEL_EDITOR "Build Birthrt as the level editor (Windows only)" OFF)
option (BIRTHRT_WADBUILDERS "Add the WAD Builders designer conveniences (Windows only)" OFF)
option (BIRTHRT_SCENE_EDITOR "Build Birthrt as the scene editor (Windows only)" OFF)
if (NOT WIN32 AND (BIRTHRT_LEVEL_EDITOR OR BIRTHRT_WADBUILDERS OR BIRTHRT_SCENE_EDITOR))
	message (FATAL_ERROR "The Birthright editors are Win32 dialog code - Windows only.")
endif ()
if (BIRTHRT_LEVEL_EDITOR)
	target_sources (Birthrt PRIVATE "EDIT.C" "EDIT.H" "SAVEPWAD.C" "SAVEPWAD.H")
	target_compile_definitions (Birthrt PRIVATE _EDIT)
endif ()
if (BIRTHRT_WADBUILDERS)
	target_compile_definitions (Birthrt PRIVATE _WADBUILDERS)
endif ()
if (BIRTHRT_SCENE_EDITOR)
	target_sources (Birthrt PRIVATE "DLG_EDIT.CPP" "DLG_EDIT.H")
	target_compile_definitions (Birthrt PRIVATE SCENE_EDITOR)
endif ()
