// ===========================================================================
// FOPEN.H
// Prototypes for FOPEN functions.
// ===========================================================================

#ifndef FOPEN_H
#define FOPEN_H

#if defined (__cplusplus)
extern "C" {
#endif

/* Text and sound languages. A translation lives in an overlay folder next to
   the game (FRENCH\, GERMAN\, RUSSIAN1\ = Fargus, RUSSIAN2\ = Russkiy Proekt)
   holding only its own files, laid out like the game folder. The order is the
   Options screen's and gameopt.dat's; STR_OPT_LANG_ENGLISH.. follow it. */
typedef enum {
	LANGUAGE_ENGLISH,
	LANGUAGE_FRENCH,
	LANGUAGE_GERMAN,
	LANGUAGE_RUSSIAN1,
	LANGUAGE_RUSSIAN2,				// text only: its WAVE.RES is not offered for sound
	LANGUAGE_COUNT
} GAME_LANGUAGE;

void SetDataLanguages (LONG TextLang, LONG SoundLang);	// once, before any data file opens
char const * SoundOverlay (void);	// "FRENCH\\" etc., NULL for English
LONG TextLanguage (void);			// GAME_LANGUAGE
/* What the 1997 French and German builds (_FRENCHVER, _GERMANVER) compiled in
   - a hotkey, an offset - for the text language; English for the others,
   whose releases were the English build. */
LONG LangPick (LONG English, LONG French, LONG German);

void CheckHandles ();
FILE *FileOpen( CSTRPTR pszFile, CSTRPTR pszAccess );
SHORT DiskOpen( CSTRPTR pszFile );
LONG FileAccess(CSTRPTR);
int DiskClose (SHORT handle);
int FileClose (FILE * fp);
void CloseAllFiles ();
#if defined (__cplusplus)
}
#endif
#endif
