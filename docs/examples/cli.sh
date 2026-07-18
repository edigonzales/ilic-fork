#!/bin/sh
set -eu

if [ "$#" -ne 4 ]; then
  echo "usage: cli.sh ILIC ILIC_FORMAT SOURCE_ROOT OUTPUT_DIRECTORY" >&2
  exit 2
fi

ilic=$1
ilic_format=$2
source_root=$3
output_directory=$4
models=$source_root/docs/examples/models
repository=$source_root/test/repository/fixture

mkdir -p "$output_directory"
mkdir -p "$output_directory/gml"

"$ilic" -silent -ilidirs "$models" "$models/Example.ili"
"$ilic" -silent "$models/Legacy.ili"
"$ilic" -silent -no_auto "$models/Base.ili" "$models/Example.ili"
"$ilic" -silent -repositories "$repository" -models RepositoryRoot
"$ilic" -quiet -werror -log "$output_directory/compile.log" \
  -ilidirs "$models" "$models/Example.ili"

"$ilic" -silent "$models/Legacy.ili" -o1 "$output_directory/example.ili1"
"$ilic" -silent -ilidirs "$models" "$models/Example.ili" -o23 "$output_directory/example-23.ili"
"$ilic" -silent -ilidirs "$models" "$models/Example.ili" -o24 "$output_directory/example-24.ili"
"$ilic" -silent -ilidirs "$models" "$models/Example.ili" -oIMD "$output_directory/example.imd"
"$ilic" -silent -ilidirs "$models" "$models/Example.ili" -oXSD "$output_directory/example.xsd"
"$ilic" -silent -ilidirs "$models" "$models/Example.ili" -oGML "$output_directory/gml"

"$ilic_format" "$models/Example.ili" > "$output_directory/formatted.ili"
"$ilic_format" --check "$models/Base.ili" "$models/Example.ili"

set +e
"$ilic" "$models/Invalid.ili" > "$output_directory/invalid.log" 2>&1
invalid_status=$?
"$ilic" -silent -success 7 -ilidirs "$models" "$models/Example.ili"
success_status=$?
set -e
if [ "$invalid_status" -ne 1 ]; then
  echo "expected invalid model status 1, got $invalid_status" >&2
  exit 1
fi
if [ "$success_status" -ne 7 ]; then
  echo "expected configured success status 7, got $success_status" >&2
  exit 1
fi

grep 'Invalid.ili:6:1: type MissingDomain not found' "$output_directory/invalid.log" >/dev/null
for output in compile.log example.ili1 example-23.ili example-24.ili \
  example.imd example.xsd gml/Example.xsd formatted.ili; do
  test -s "$output_directory/$output"
done
