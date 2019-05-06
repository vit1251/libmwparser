/*             itex2MML 1.4.5
 *   itex2MML.h last modified 10/2/2010
 */

#ifndef ITEX2MML_H
#define ITEX2MML_H

#define ITEX2MML_VERSION "1.4.5"

#ifdef __cplusplus
 "C" {
#endif

  /* Step 1. Parse a buffer with itex source; return value is mathml, or 0 on failure (e.g., parse error).
   */
   char * itex2MML_parse (const char * buffer, unsigned long length);

  /* Step 2. Free the string from Step 1.
   */
   void   itex2MML_free_string (char * str);


  /* Alternatively, to filter generic source and converting embedded equations, use:
   */
   int    itex2MML_filter (const char * buffer, unsigned long length);

   int    itex2MML_html_filter (const char * buffer, unsigned long length);
   int    itex2MML_strict_html_filter (const char * buffer, unsigned long length);


  /* To change output methods:
   *
   * Note: If length is 0, then buffer is treated like a string; otherwise only length bytes are written.
   */
   void (*itex2MML_write) (const char * buffer, unsigned long length); /* default writes to stdout */
   void (*itex2MML_write_mathml) (const char * mathml);                /* default calls itex2MML_write(mathml,0) */
   void (*itex2MML_error) (const char * msg);                          /* default writes to stderr */


  /* Other stuff:
   */
   void   itex2MML_setup (const char * buffer, unsigned long length);

   void   itex2MML_restart ();

   char * itex2MML_copy_string (const char * str);
   char * itex2MML_copy_string_extra (const char * str, unsigned extra);
   char * itex2MML_copy2 (const char * first, const char * second);
   char * itex2MML_copy3 (const char * first, const char * second, const char * third);
   char * itex2MML_copy_escaped (const char * str);

   char * itex2MML_empty_string;

   int    itex2MML_lineno;

   int    itex2MML_rowposn;
   int    itex2MML_displaymode;

#ifdef __cplusplus
}
#endif

#endif /* ! ITEX2MML_H */
