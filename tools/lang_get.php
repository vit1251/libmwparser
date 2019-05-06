#!/usr/bin/php -q 
<?php

$rtl = array("ar", "arc", "dv", "fa", "fa-af", "glk", "ha", "he", "ks",
             "ku", "mzn", "prd", "ps", "sd" , "ur", "yi");

function wiki_lang_functions() {
   return "
const char *wiki_lang_magic_word_get(Eina_Hash *hash, const char *lang, const char *word)
{
   const Wiki_Lang *wl = NULL;
   const char *ret = NULL;

   if(! hash) return ret;
   if(! lang) return ret;
   if(! word) return ret;

   wl = eina_hash_find(hash, lang);
   if(wl) {
      const Wiki_Lang_Magic_Word *mw = wl->magic_words;
      while(mw && mw->locale && ret == NULL) {
         if(strcmp(mw->locale, word) == 0)
            ret = mw->name;
         mw++;
      }
   }

   return ret;
}

const char *wiki_lang_message_get(Eina_Hash *hash, const char *lang, const char *name)
{
   const Wiki_Lang *wl = NULL;
   const char *ret = NULL;

   if(! hash) return ret;
   if(! lang) return ret;
   if(! name) return ret;

   wl = eina_hash_find(hash, lang);
   if(wl) {
      const Wiki_Lang_Message *lm = wl->messages;
      while(lm && lm->name && ret == NULL) {
         if(strcmp(lm->name, name) == 0)
            ret = lm->message;
         lm++;
      }
   }

   return ret;
}

Eina_Hash *wiki_languages_init(void)
{
   Eina_Hash *wiki_languages_hash = NULL;

   const Wiki_Lang *lang = wiki_languages;

   wiki_languages_hash = eina_hash_string_superfast_new(NULL);

   if(wiki_languages_hash) {
      while(lang->code) {
         eina_hash_add(wiki_languages_hash, lang->code, lang);
         lang++;
      }
   }

   return wiki_languages_hash;
}
";
}

function usage($code) {
  echo "Usage: lang_get.php mediawiki_dir output_dir

Convert Mediawiki languages translation to C. Output_dir should be
\$TOP_DIR/src/lib/
";
  exit($code);
}

function convert_messages_to_po($inc, $lang, $license, $pfunc) {
   $ret = NULL;
   if($magicWords) unset($magicWords);
   if($messages) unset($messages);
   
   @include("$inc");

   $date = date("Y-m-d H:i+0000");

   $ret = "msgid \"\"
msgstr \"\"
\"Project-Id-Version: libmwparser\\n\"
\"POT-Creation-Date: $date\\n\"
\"PO-Revision-Date: $date\\n\"
\"Last-Translator: \\n\"
\"Language-Team: \\n\"
\"Language: $lang\\n\"
\"MIME-Version: 1.0\\n\"
\"Content-Type: text/plain; charset=UTF-8\\n\"
\"Content-Transfer-Encoding: 8bit\\n\"

";
   if(is_array($magicWords)) {
      foreach($magicWords as $k => $v) {
         array_shift($v);
         $msgstr = array_pop($v);
         $msgstr = addcslashes($msgstr, "\0..\37\"\\");
         foreach($v as $msgid) {
            $msgid = addcslashes($msgid, "\0..\37\"\\");
            $ret .= "msgid  \"$msgid\"\n";
            $ret .= "msgstr \"$msgstr\"\n\n";
         }
      }

      foreach($messages as $k => $v) {
         $k = addcslashes($k, "\0..\37\"\\");
         $v = addcslashes($v, "\0..\37\"\\");
         $ret .= "msgid  \"$k\"\n";
         $ret .= "msgstr  \"$v\"\n\n";
      }
   }

   if(is_array($license)) {
      foreach($license as $k => $v) {
         $k = addcslashes($k, "\0..\37\"\\");
         $v = addcslashes($v, "\0..\37\"\\");
         $ret .= "msgid  \"$k\"\n";
         $ret .= "msgstr  \"$v\"\n\n";
      }
   }

   if(is_array($pfunc)) {
      foreach($pfunc as $k => $v) {
         if($k[0] != "#") $k = "#" . $k;
         array_shift($v);
         foreach($v as $msgid) {
           if($msgid[0] != "#") $msgid = "#" . $msgid;
           if($msgid != $k) {
              $ret .= "msgid  \"$msgid\"\n";
              $ret .= "msgstr  \"$k\"\n\n";
           }
         }
      }
   }

   return $ret;
}

$path = dirname(realpath($_SERVER['argv'][0]));
include("$path/Wikipedia_license_text.php");
if(is_array($messages))
   $licenses = $messages;
include("$path/ParserFunctions.i18n.magic.php");
if(is_array($magicWords))
   $pfunc = $magicWords;

$mwdir = $_SERVER['argv'][1];
if(! $mwdir || ! is_dir($mwdir)) usage(1);
$outdir = $_SERVER['argv'][2];
if(! $outdir || ! is_dir($outdir)) usage(1);

$wlMagicWord = array();
$wlMessages = array();

$file = "$mwdir/languages/Names.php";
include($file);
if(! is_array($wgLanguageNames)) {
   echo "wgLanguageNames not found in $file\n";
   usage(1);
}

$fwl = fopen("$outdir/wiki_lang.c", "w");
if(! $fwl) {
   echo "Can't open wiki_lang.c for writting\n";
   usage(1);
}

$wikiLang = "/* Lang list converted from wikimedia Names.php */
#include <Eina.h>
#include <stdlib.h>

#include \"wiki_lang.h\"
";

foreach($wgLanguageNames as $k => $v) {
   $name = ucfirst($k);
   $name = strtr($name, "-", "_");
   $lang = strtr($k, "-", "_");
   $ret = convert_messages_to_po("$mwdir/languages/messages/Messages$name.php", $lang, $licenses[$k], $pfunc[$k]);
   if($ret) {
      $out = fopen("$path/../po/$lang.po", "w");
      if($out) {
         fwrite($out, $ret);
         fclose($out);
      } else
        echo "can't open file languages/Messages$name.h for writting\n";
   }
}

$wikiLang .= "
static const Wiki_Lang wiki_languages[] = {
";
foreach($wgLanguageNames as $k => $v) {
   $tmp = sprintf("   {\"%s\", \"\", \"%s\", \"%s\", %s, %s},\n",
       $k, ((in_array($k, $rtl)) ? "rtl" : "ltr"), $v,
       (isset($wlMagicWord[$k])) ? $wlMagicWord[$k] : "NULL",
       (isset($wlMessages[$k])) ? $wlMessages[$k] : "NULL"
       );
   $wikiLang .= $tmp;    
}
$wikiLang .= "   {NULL, NULL, NULL, NULL, NULL, NULL}
};
";
     
$wikiLang .= wiki_lang_functions();
fwrite($fwl, $wikiLang);
fclose($fwl);

?>
