project('libmwparser', 'c', version: '')

sources = [
    'src/expr.tab.c',
#    'src/itex2MML_yy.tab.c',
    'src/itex2MML.tab.c',
    'src/itex2MML_yy.c',
    'src/wiki_lang.c',
    'src/lexer.c',
    'src/entities.c',
    'src/parser.c',
    'src/parser_extension_private.c',
    'src/parser_tag_extensions.c',
    'src/parser_extension_functions.c',
    'src/parser_extension_magic_words.c',
    'src/parser_extension_time.c',
    'src/parser_extension_stats.c',
    'src/parser_extension_builtin.c',
    'src/parser_output_xhtml.c',
    'src/parser_title.c',
    'src/util.c',
    'src/icu_eina_hash.c',
    'src/pcre_replace.c',
    'src/md5.c',
]

static_library('libmwparser', sources)
