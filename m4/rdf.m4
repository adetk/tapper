<!-- m4/rdf.sh -->
m4_dnl
m4_divert(-1)

#   ---------------------------------------------------------------------- copyright and license ---
#
#   File: m4/rdf.m4
#
#   Copyright 🄯 2014, 2016—2017, 2019—2023 Van de Bugger.
#
#   This file is part of Tapper.
#
#   Tapper is free software: you can redistribute it and/or modify it under the terms of the GNU
#   General Public License as published by the Free Software Foundation, either version 3 of the
#   License, or (at your option) any later version.
#
#   Tapper is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
#   even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   General Public License for more details.
#
#   You should have received a copy of the GNU General Public License along with Tapper.  If not,
#   see <https://www.gnu.org/licenses/>.
#
#   SPDX-License-Identifier: GPL-3.0-or-later
#
#   ---------------------------------------------------------------------- copyright and license ---

#
#   M4 macros for embedding RDFa attributes into markdown source. Abot RDFa see:
#
#       https://www.w3.org/TR/rdfa-lite/
#       https://www.w3.org/TR/xhtml-rdfa-primer/
#
#   Usage example:
#
#       RDF_DESCRIPTION({{ It is made for RDF_KEYWORD({{ GNU/Linux }}) based on RDF_KEYWORD({{
#       glibc }}). RDF_OFFER({{ Tapper is RDF_PRICE({{ 0.0 }},{{ free }}) software. }}) }})
#
#       ---
#       description: "RDF_DESCRIPTION"
#       keywords: "RDF_KEYWORDS"
#       ...
#

m4_changequote({{, }})


#   _rdf_ignore()
#   ------------------------------------------------------------------------------------------------
#   Ignore all the arguments, expand to empty string.

m4_define({{_rdf_ignore}}, {{}})


#   _rdf_line(STRING)
#   ------------------------------------------------------------------------------------------------
#   Replaces each new line in STRING with a single space. Returns quoted string.

m4_define({{_rdf_line}}, {{m4_patsubst({{{{$1}}}}, {{
}}, {{ }})}})


#   _rdf_ltrim(STRING)
#   ------------------------------------------------------------------------------------------------
#   Trims leading whitespaces from STRING. Non-leading whitespace remains intact. Returns quoted
#   string.

m4_define({{_rdf_ltrim}}, {{m4_patsubst({{{{$1}}}}, {{\`\(..\)\s*}}, {{\1}})}})


#   _rdf_rtrim(STRING)
#   ------------------------------------------------------------------------------------------------
#   Trims trailing whitespaces from STRING. Non-trailing whitespace remains intact. Returns quoted
#   string.

m4_define({{_rdf_rtrim}}, {{m4_patsubst({{{{$1}}}}, {{\s*\(..\)\'}}, {{\1}})}})


#   _rdf_trim(STRING)
#   ------------------------------------------------------------------------------------------------
#   Trims both leading and trailing whitespaces from STRING. Inner whitespace remains intact.
#   Returns quoted string.

m4_define({{_rdf_trim}}, {{_rdf_ltrim(_rdf_rtrim({{$1}}))}})


#   _rdf_norm(STRING)
#   ------------------------------------------------------------------------------------------------
#   Trims both leading and trailing whitespaces. Every double tilda followed by newline is deleted
#   with the following whitespaces. Returns quoted string.

m4_define({{_rdf_norm}}, {{_rdf_trim(m4_patsubst({{{{$1}}}}, {{~~
\s*}}, {{{{}}}}))}})


#   _rdf_anorm(STRING, ...)
#   ------------------------------------------------------------------------------------------------
#   Apply _rdf_norm to all arguments.

m4_define(
    {{_rdf_anorm}},
    {{m4_ifelse(
        {{$#}},
        {{0}},
        {{}},
        {{$#}},
        {{1}},
        {{_rdf_norm({{$1}})}},
        {{_rdf_norm({{$1}}), _rdf_anorm(m4_shift($@))}})}})


#   _rdf_demark(STRING)
#   ------------------------------------------------------------------------------------------------
#   Strips markdown formatting from string. It deletes footnote references and then deletes all
#   square brackets, asterisks, and grave accents. Returns quoted string.

m4_define(
    {{_rdf_demark}},
    {{m4_translit(
        m4_patsubst(
            {{{{{{$1}}}}}},
            {{\[\^[a-z-]+\]}},
            {{}}),
        {{[]*`}})}})m4_dnl   # `


#   _rdf_dquote(STRING)
#   ------------------------------------------------------------------------------------------------
#   Returns double-quoted STRING.

m4_define({{_rdf_dquote}}, {{{{{{$1}}}}}})


#   _rdf_no_prefix(STRING, PREFIX)
#   ------------------------------------------------------------------------------------------------
#   Drops PREFIX from STRING, if any. PREFIX is a regular expression (do not include \` anchor!).
#   Returns quoted string.

m4_define({{_rdf_no_prefix}}, {{m4_patsubst({{{{$1}}}}, {{\`\(..\)$2}}, {{\1}})}})


#   _rdf_tolower(STRING)
#   ------------------------------------------------------------------------------------------------
#   Converts STRING to lower case. Returns quoted string.

m4_define({{_rdf_tolower}}, {{m4_translit({{{{$1}}}}, {{A-Z}}, {{a-z}})}})


#   _rdf_toupper(STRING)
#   ------------------------------------------------------------------------------------------------
#   Converts STRING to upper case. Returns quoted string.

m4_define({{_rdf_toupper}}, {{m4_translit({{{{$1}}}}, {{a-z}}, {{A-Z}})}})


#   _rdf_tocamel( STRING )
#   ------------------------------------------------------------------------------------------------
#   Converts STRING to camelCase (with first letter in lower case). Returns quoted string.

m4_define({{_rdf_tocamel}}, {{$0_(_rdf_tolower({{$1}}), {{abcdefghijklmnopqrstuvwxyz}})}})
m4_define({{_rdf_tocamel_}},
    {{m4_ifelse(
        {{$2}}, {{}}, {{$1}},
        {{$0(
            $0_({{$1}}, m4_substr({{$2}}, 0, 1), _rdf_toupper(m4_substr({{$2}}, 0, 1))),
            m4_substr({{$2}}, 1))}})}})
m4_define({{_rdf_tocamel__}}, {{m4_patsubst({{{{$1}}}}, {{_$2}}, {{$3}})}})


#   ================================================================================================
#   m4 macro wrappers.
#   ================================================================================================

#   Each of these _rdf_* macros normalizes every argument and invokes appropriate m4_ macro.

m4_define({{_rdf_define}}, {{m4_define(_rdf_anorm($@))}})

_rdf_define({{ _rdf_ifdef    }},{{ m4_ifdef(    _rdf_anorm($@)) }})
_rdf_define({{ _rdf_ifelse   }},{{ m4_ifelse(   _rdf_anorm($@)) }})
_rdf_define({{ _rdf_pushdef  }},{{ m4_pushdef(  _rdf_anorm($@)) }})
_rdf_define({{ _rdf_popdef   }},{{ m4_popdef(   _rdf_anorm($@)) }})
_rdf_define({{ _rdf_divert   }},{{ m4_divert(   _rdf_anorm($@)) }})
_rdf_define({{ _rdf_undivert }},{{ m4_undivert( _rdf_anorm($@)) }})


#   ================================================================================================
#   m4sugar-like macros.
#   ================================================================================================

#   _rdf_expand
#   ------------------------------------------------------------------------------------------------
#   This is simpler variant of m4sugar m4_expand. _rdf_expand does not handle unbalanced ().

_rdf_define({{ _rdf_expand }},{{ $0_(--==<<($1)>>==--) }})
_rdf_define({{ _rdf_expand_ }},{{
    m4_changequote({{--==<<(}}, {{)>>==--}})$1m4_changequote({{, }})
}})


#   RDF_ENABLE
#   ------------------------------------------------------------------------------------------------
#   If 1, RDF_* macros generate RDFa attributes. Otherwise, plain text is generated. By default
#   RDFa attributes are enabled. RDFa attributes can be turned off either from command line:
#       $ m4 -D RDF_ENABLE=0 ...
#   or from code:
#       m4_define({{RDF_ENABLE}},{{0}})

_rdf_ifdef({{ RDF_ENABLE }},{{ }},{{ _rdf_define({{ RDF_ENABLE }},{{ 1 }}) }})


#   _rdf_entity_level
#   ------------------------------------------------------------------------------------------------
#   Initially 0, which means "top-level entity". Whithin an RDFa entity (element with `typeof`
#   attribute) the value is temporary bumped by 1.

_rdf_define({{ _rdf_entity_level }},{{ 0 }})


#   _rdf_divnum_kword
#   ------------------------------------------------------------------------------------------------
#   Number of diversion to accumulate keywords in. If < 0, keywords accumulation is disabled.

_rdf_ifdef({{ _rdf_divnum_kword }},{{ }},{{ _rdf_define({{ _rdf_divnum_kword }},{{ 1001 }}) }})

#   _rdf_divnum_dexcr
#   ------------------------------------------------------------------------------------------------
#   Number of diversion to write plain text description to.

_rdf_ifdef({{ _rdf_divnum_descr }},{{ }},{{ _rdf_define({{ _rdf_divnum_descr }},{{ 1002 }}) }})

#   _rdf_prop( CALLER, TYPEOF, [CONTENT_ATTR,] CONTENT_ELEM )
#   ---------------------------------------------------------------------------------------------
#   Work horse of all RDF_* macros. Do not use it directly in markdown code. IF RDF_ENABLE is 1,
#   the macro generates html `span` element (<span property="PROPERTY">...</span>). PROPERTY is
#   calculated from CALLER: RDF_ prefix dropped, result converted to camelCase. If TYPEOF is not
#   empty, `span` tag will have `typeof` attribute: <span ... typeof="TYPEOF">. The next two
#   arguments defines value of `content` attribute and content of the `span` element: <span ...
#   content="CONTENT_ATTR">CONTENT_ELEM</span>. If CONTENT_ATTR is empty, `content` attribute will
#   not be generated.
#   &#8204; — zero width non-joiner. tidy does not like empty spans.

_rdf_define({{ _rdf_prop }},{{
    _rdf_pushdef({{ _rdf_entity_level }}, _rdf_entity_level )~~
    _rdf_ifelse({{
        }}RDF_ENABLE{{
    }},{{
        1
    }},{{
        <span property="_rdf_tocamel(_rdf_no_prefix(_rdf_trim({{ $1 }}), {{RDF_}}))"~~
        _rdf_ifelse({{
            $2
        }},{{
        }},{{
        }},{{
            {{ }}~~
            typeof="_rdf_trim({{ $2 }})"~~
            _rdf_define({{ _rdf_entity_level }}, m4_eval( _rdf_entity_level + 1 ))~~
        }})~~
        _rdf_ifelse({{
            $4
        }},{{
        }},{{
        }},{{
            _rdf_ifelse({{
                $3
            }},{{
            }},{{
            }},{{
                {{ }}content="_rdf_trim( _rdf_demark({{ $3 }}) )"
            }})
        }})~~
        >
    }})~~
    _rdf_ifelse({{
        $4
    }},{{
    }},{{
        $3
    }},{{
        _rdf_ifelse({{
            $4
        }},{{
            -
        }},{{
            &#8204;
        }},{{
            $4
        }})
    }})~~
    _rdf_ifelse({{
        }}RDF_ENABLE{{
    }},{{
        1
    }},{{
        </span>
    }})~~
    _rdf_popdef({{ _rdf_entity_level }})~~
}})

#   RDF_XXX( [CONTENT_ATTR,] CONTENT_ELEM )
#   ------------------------------------------------------------------------------------------------

_rdf_define({{ RDF_APPLICATION_CATEGORY }},{{ _rdf_prop({{ $0     }},{{          }},{{ $1 }},{{ $2 }}) }})
_rdf_define({{ RDF_AUDIENCE             }},{{ _rdf_prop({{ $0     }},{{ Audience }},{{ $1 }},{{ $2 }}) }})
_rdf_define({{ RDF_AUDIENCE_TYPE        }},{{ _rdf_prop({{ $0     }},{{          }},{{ $1 }},{{ $2 }}) }})
_rdf_define({{ RDF_AUTHOR               }},{{ _rdf_prop({{ $0     }},{{ Person   }},{{ $1 }},{{    }}) }})
_rdf_define({{ RDF_COPYRIGHT_HOLDER     }},{{ _rdf_prop({{ $0     }},{{ Person   }},{{ $1 }},{{ $2 }}) }})
_rdf_define({{ RDF_COPYRIGHT_YEAR       }},{{ _rdf_prop({{ $0     }},{{          }},{{ $1 }},{{ $2 }}) }})
_rdf_define({{ RDF_COUNTRIES_SUPPORTED  }},{{ _rdf_prop({{ $0     }},{{          }},{{ $1 }},{{ $2 }}) }})
_rdf_define({{ RDF_DATE_PUBLISHED       }},{{ _rdf_prop({{ $0     }},{{          }},{{ $1 }},{{ $2 }}) }})
_rdf_define({{ RDF_DOWNLOAD_URL         }},{{ _rdf_prop({{ $0     }},{{          }},{{ $1 }},{{ $2 }}) }})
_rdf_define({{ RDF_EMAIL                }},{{ _rdf_prop({{ $0     }},{{          }},{{ $1 }},{{ $2 }}) }})
_rdf_define({{ RDF_FILE_FORMAT          }},{{ _rdf_prop({{ $0     }},{{          }},{{ $1 }},{{ $2 }}) }})
_rdf_define({{ RDF_HEADLINE             }},{{ _rdf_prop({{ $0     }},{{          }},{{ $1 }},{{ $2 }}) }})
_rdf_define({{ RDF_LICENSE              }},{{ _rdf_prop({{ $0     }},{{          }},{{    }},{{ _rdf_trim({{ $1 }}) }}) }})
_rdf_define({{ RDF_NAME                 }},{{ _rdf_prop({{ $0     }},{{          }},{{ $1 }},{{ $2 }}) }})
_rdf_define({{ RDF_OFFER                }},{{ _rdf_prop({{ offers }},{{ Offer    }},{{ $1 }},{{    }}) }})
_rdf_define({{ RDF_PRICE                }},{{ _rdf_prop({{ $0     }},{{          }},{{ $1 }},{{ $2 }}) }})
_rdf_define({{ RDF_PRICE_CURRENCY       }},{{ _rdf_prop({{ $0     }},{{          }},{{ $1 }},{{ $2 }}) }})
_rdf_define({{ RDF_RELEASE_NOTES        }},{{ _rdf_prop({{ $0     }},{{          }},{{ $1 }},{{ $2 }}) }})
_rdf_define({{ RDF_SOFTWARE_VERSION     }},{{ _rdf_prop({{ $0     }},{{          }},{{ $1 }},{{ $2 }}) }})
_rdf_define({{ RDF_URL                  }},{{ _rdf_prop({{ $0     }},{{          }},{{ $1 }},{{ $2 }}) }})


#   RDF_OPERATING_SYSTEM( [CONTENT_ATTR,] CONTENT_ELEM )
#   ------------------------------------------------------------------------------------------------
#   RDF_OPERATING_SYSTEM is a bit special: it generates `operatingSystem` and `keywords` microdata.

_rdf_define({{ RDF_OPERATING_SYSTEM }},{{
    _rdf_prop({{ $0 }},{{ }},{{ $1 }},{{ RDF_KEYWORD({{ $1 }},{{ $2 }}) }})
}})


#   RDF_KEYWORD( [CONTENT_ATTR,] CONTENT_ELEM )
#   RDF_KEYWORDS
#   ------------------------------------------------------------------------------------------------
#   RDF_KEYWORD is special: it generates `keywords` property *and* (in case of top-level entity)
#   appends CONTENT_ELEM to _rdf_divnum_kword diversion (if diversion number >= 0).
#   RDF_KEYWORDS undiverts plain comma-separated list of keywords.

_rdf_define({{ RDF_KEYWORD }},{{
    _rdf_prop({{ keywords }},{{ }},{{ $1 }},{{ $2 }})~~
    _rdf_ifelse(
        m4_eval( _rdf_entity_level == 0 && _rdf_divnum_kword >= 0 )
    ,{{
        1
    }},{{
        _rdf_pushdef({{ _rdf_divnum }}, m4_divnum )~~
        _rdf_pushdef({{ RDF_ENABLE }})~~
        m4_divert(_rdf_divnum_kword)~~
        _rdf_line(_rdf_trim( _rdf_demark( _rdf_expand({{ $1 }}) ) ))~~
        , ~~
        _rdf_divert( _rdf_divnum )~~
        _rdf_popdef({{ RDF_ENABLE }})~~
        _rdf_popdef({{ _rdf_divnum }})
    }})
}})
_rdf_define({{ RDF_KEYWORDS }},{{ _rdf_undivert( _rdf_divnum_kword ) }})


#   RDF_DESCRIPTION( CONTENT_ELEM )
#   RDF_DESCRIPTION
#   ------------------------------------------------------------------------------------------------
#   RDF_DESCRIPTION is special: it generates `description` property *and* (in case of top-level
#   entity) saves plain text version of CONTENT_ELEM to _rdf_divnum_descr diversion (if diversion
#   number >= 0). RDF_DESCRIPTION with no arguments undiverts plain text description.

_rdf_define({{ RDF_DESCRIPTION }},{{
    _rdf_ifelse({{
        $#
    }},{{
        0
    }},{{
        _rdf_undivert( _rdf_divnum_descr )
    }},{{
        _rdf_prop({{ $0 }},{{ }},{{ $1 }},{{ }})~~
        _rdf_ifelse({{
            }}m4_eval( _rdf_entity_level == 0 && _rdf_divnum_descr >= 0 ){{
        }},{{
            1
        }},{{
            _rdf_pushdef({{ RDF_ENABLE }})~~
            _rdf_pushdef({{ _rdf_divnum_kword }},{{ -1 }})~~
            _rdf_pushdef({{ _rdf_divnum }}, m4_divnum )~~
            _rdf_divert( _rdf_divnum_descr )~~
            _rdf_line(_rdf_trim( _rdf_demark( _rdf_expand({{ $1 }}) ) ))~~
            _rdf_divert( _rdf_divnum )~~
            _rdf_popdef({{ _rdf_divnum }} )~~
            _rdf_popdef({{ _rdf_divnum_kword }})~~
            _rdf_popdef({{ RDF_ENABLE }})
        }})
    }})
}})


#   RDF_COMMENT( CONTENT )
#   ------------------------------------------------------------------------------------------------
#   RDF_COMMENT creates a valid HTML comment. Tidy complains if a comment includes ajacent dashes.
#   This macro replaces series of adjacent dashes (U+002D) with series of Unicode minus signes
#   (U+2212). Unicode minus sign looks like ASCII minus, but it is different character which does
#   not cause tidy errors.

_rdf_define({{ RDF_COMMENT }},{{
    {{<!--}} m4_patsubst({{$1}},{{--+}},{{m4_patsubst({{\&}},{{-}},{{−}})}}) {{-->}}
}})

_rdf_define({{ RDF_DISCARD }},{{}})

m4_changecom({{<!--}}, {{-->}})

m4_divert(0)
m4_dnl
<!-- end of file -->
