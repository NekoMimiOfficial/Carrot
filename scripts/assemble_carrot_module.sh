#!/usr/bin/env bash
set -e

cd "$(dirname "$0")/../components/nekko/head" || exit 1

OUTPUT="../../module/carrot_module.h"
TMP_OUT="carrot_module_tmp.h"

echo "#pragma once" > "$TMP_OUT"

grep -h "^[[:space:]]*#[[:space:]]*include[[:space:]]*<" *.h */*.h 2>/dev/null | sort -u >> "$TMP_OUT"
echo "" >> "$TMP_OUT"

VISITED=":"

process_header() {
  local file="$1"

  if [[ -f "$file" && "$VISITED" != *":$file:"* ]]; then
    VISITED="$VISITED$file:"

    while IFS= read -r line || [[ -n "$line" ]]; do
      if [[ "$line" =~ ^[[:space:]]*#[[:space:]]*include[[:space:]]*\"([^\"]+)\" ]]; then
        local target="${BASH_REMATCH[1]}"

        if [[ -f "$target" ]]; then
          process_header "$target"
        elif [[ -f "utils/$target" ]]; then
          process_header "utils/$target"
        fi
      elif [[ ! "$line" =~ ^[[:space:]]*#[[:space:]]*include[[:space:]]*\< ]] && \
           [[ ! "$line" =~ ^[[:space:]]*#[[:space:]]*pragma[[:space:]]+once ]]; then
        echo "$line"
      fi
    done < "$file"
  fi
}

# process_header "carrot_module.h" | sed -e 's|//.*||' -e '/^[[:space:]]*$/d' >> "$TMP_OUT"
# process_header "carrot_module.h" | sed -e 's|//.*||' -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//' -e '/^$/d' >> "$TMP_OUT"
process_header "carrot_module.h" | \
    sed -e 's|//.*||' -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//' -e '/^$/d' | \
    awk '
    BEGIN { in_macro = 0 }
    /^#/ { 
        if (!in_macro) printf "\n"
        print $0
        in_macro = ($0 ~ /\\$/)
        next 
    }
    in_macro { 
        print $0
        in_macro = ($0 ~ /\\$/)
        next 
    }
    { 
        printf "%s ", $0 
    }
    END { 
        printf "\n" 
    }
    ' | sed '/^$/d' >> "$TMP_OUT"

mv "$TMP_OUT" "$OUTPUT"
