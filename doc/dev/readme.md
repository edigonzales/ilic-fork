# ilic Developer Documentation

> The current user, architecture and embedding documentation is maintained in
> [`../../docs/README.md`](../../docs/README.md). This directory only retains
> the still applicable coding and embedding notes.

## Introduction
Welcome to the ilic developer documentation. ilic is an INTERLIS compiler written in c++. 
ilic was completely built from scratch, this is no c++ port of the 
[Java INTERLIS compiler](https://github.com/claeis/ili2c).

## Project Goals
+ fast INTERLIS compiler without any runtime or installation requirements (except OS)
+ one single executable
+ standard c++ without external libraries or frameworks (except ANTLR for parse tree generation)
+ reusable ANTLR grammars without embedded c++ code
+ simple and easy to extend source code
+ documented api for developers
+ supported input formats: INTERLIS 1.0, 2.3 and 2.4
+ supported output formats: INTERLIS 1.0, 2.3 and 2.4, imd, xsd, gml

## Build und Generatoren

Für den aktuellen lokalen Build, den Windows-CI-Stack und die Erklärung der
ANTLR-Runtime siehe [Build und Installation](../../docs/build-und-installation.md),
insbesondere [ANTLR-Runtime](../../docs/build-und-installation.md#antlr-runtime)
und [Windows-Build-Stack](../../docs/build-und-installation.md#windows-build-stack).

Wenn ANTLR-Grammatiken unter `/source/input/parser/grammar` geändert werden,
müssen die C++-Parserdateien mit dem passenden ANTLR-4.7.1-Generator neu erzeugt
werden. Die aktuelle, nicht überschreibende Prüfung ist unter
[Parser-Regeneration](../../docs/build-und-installation.md#parser-regeneration)
beschrieben.

## Additional Information
+ [current architecture and public APIs](../../docs/README.md)
+ [coding conventions](./codingconventions.md)
+ [native C ABI, JSON protocol and WebAssembly embedding](./embedding.md)
