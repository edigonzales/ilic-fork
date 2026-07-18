//
//  start of grammar
//

parser grammar Ili2Parser;

@members {
   bool ili23=false; 
   bool ili24=false;
}

options
{
   tokenVocab = Ili2Lexer;
}

decimal
   : DEC
   | POSNUMBER
   | NUMBER
   ;
   
path
   : (INTERLIS DOT)? (SIGN | URI | REFSYSTEM | BOOLEAN | HALIGNMENT | VALIGNMENT | METAOBJECT)
   | (INTERLIS DOT)? NAME (DOT NAME)*
   ;
   
interlis2Def
   : INTERLIS 
     ( iliversion=ILI23 {ili23=true;}
     | iliversion=ILI24 {ili24=true;}
     )
     SEMI (modelDef)* EOF
   ;

modelDef
   : (contracted=CONTRACTED)? 
     (refsystemmodel=REFSYSTEM | symbologymodel=SYMBOLOGY | typemodel=TYPE)? 
     MODEL modelname1=NAME (LPAREN language=NAME RPAREN)?
     ({ili24}? NOINCREMENTALTRANSFER)?
     ATT issuerurl=STRING 
     VERSION modelversion=STRING (modelversion_expl=EXPLANATION)?
     (TRANSLATION OF translationOf=NAME LBRACE translationOfVersion=STRING RBRACE)?
     EQUAL
     ({ili24}? CHARSET iananame=STRING SEMI)?
     ({ili24}? XMLNS xmlns=STRING SEMI)?      
     (importDef)*
     (metaDataBasketDef
     | unitDecl
     | functionDef
     | lineFormTypeDef
     | domainDef
     | {ili24}? contextDef
     | runTimeParameterDef
     | classDef
     | structureDef
     | topicDef 
     )*
     END modelname2=NAME DOT
   ;

importDef
   : IMPORTS importing (COMMA importing)* SEMI
   ;

importing
   : UNQUALIFIED? (INTERLIS | NAME)
   ;
   
topicDef
   : VIEW? TOPIC topicname1=NAME
     properties? // ABSTRACT|FINAL
     (EXTENDS topicbase=path)?
     EQUAL
     (BASKET OID AS basketOid=path SEMI)?
     (OID AS topicOid=path SEMI)?
     (DEPENDS ON topicPath (COMMA topicPath)* SEMI)*
     ({ili24}? deferredGenerics)?
     (metaDataBasketDef
     |unitDecl
     |functionDef
     |domainDef
     |classDef
     |structureDef
     |associationDef
     |constraintsDef
     |viewDef
     |graphicDef
     )*
     END topicname2=NAME SEMI
   ;
   
topicPath
   : path
   ;

deferredGenerics
   : DEFERRED GENERICS path (COMMA path)* SEMI
   ;
   
classDef
   : CLASS classname1=NAME properties? // ABSTRACT|EXTENDED|FINAL
     (EXTENDS classbase=path)? EQUAL
     ((OID AS classoid=path | NO OID) SEMI )?
     classOrStructureDef
     END classname2=NAME SEMI
   ;

structureDef
   : STRUCTURE structurename1=NAME properties? // ABSTRACT|EXTENDED|FINAL
     (EXTENDS structurebase=path)? EQUAL
     classOrStructureDef
     END structurename2=NAME SEMI
   ;
               
classOrStructureDef
   : ATTRIBUTE? 
     attributeDef*
     constraintDef*
     (PARAMETER parameterDef*)?
   ;

attributeDef
   : (CONTINUOUS? SUBDIVISION)? attributname=NAME
     properties? // ABSTRACT|EXTENDED|FINAL
     COLON attrTypeDef
     (COLONEQUAL factor (COMMA factor)*)? SEMI
   ;

attrTypeDef
   : MANDATORY attrType?
   | attrType
   | bagOrListType
   ;
   
bagOrListType
   : (BAG | LIST) cardinality? OF ({ili23}? restrictedRef | {ili24}? attrType)
   ;
      
attrType
   : type
   | path
   | referenceAttr
   | restrictedRef
   ;

referenceAttr
   : REFERENCE TO (LPAREN EXTERNAL RPAREN)? restrictedRef
   ;

restrictedRef
   : (typeref=path | ANYCLASS | ANYSTRUCTURE)
     restriction?
   ;

restriction
   : RESTRICTION LPAREN path (SEMI path)* RPAREN
   ;

associationDef
   : ASSOCIATION (associationname1=NAME)?
     properties? // ABSTRACT|EXTENDED|FINAL|OID
     (EXTENDS associationRef)?
     (DERIVED FROM renamedViewableRef)? EQUAL
     ((OID AS assocoid=path | NO OID) SEMI)?
     (roleDef)*
     ATTRIBUTE? (attributeDef)*
     (CARDINALITY EQUAL cardinality SEMI)?
     (constraintDef)*
     END (associationname2=NAME)? SEMI
   ;

associationRef
   : ((modelname=NAME DOT)? topicname=NAME DOT)? associationname=NAME
   ;

roleDef
   : rolename=NAME 
     properties? // ABSTRACT|EXTENDED|FINAL|HIDING|ORDERED|EXTERNAL
     (ASSOCIATE | AGGREGATE | COMPOSITE ) cardinality?
     restrictedRef (OR restrictedRef)*
     (COLONEQUAL role=factor)? SEMI
   ;

cardinality
   : LCURLY (STAR | min=POSNUMBER (DOTDOT (max=POSNUMBER | STAR))?) RCURLY
   ;

domainDef
   : ILIDOMAIN domainType*
   ;
                
domainType
   : domainname=NAME properties? // ABSTRACT|GENERIC|FINAL
     (EXTENDS basedomain=path)? EQUAL
     (MANDATORY type? | type) 
     ({ili24}? CONSTRAINTS NAME COLON expression (COMMA NAME COLON expression)*)?
     SEMI
   ;
                
type
   : baseType 
   | lineType
   ;
   
baseType
   : textType
   | enumerationType
   | enumTreeValueType
   | alignmentType
   | booleanType
   | numericType
   | formattedType
   | {ili24}? dateTimeType
   | coordinateType
   | oIDType
   | blackboxType
   | classRefType
   | attributePathType
   ;

constant
   : UNDEFINED
   | numericConst
   | textConst
   | formattedConst
   | enumConst
   | classConst
   | attributePathConst
   ;

textType
   : MTEXT (STAR maxlength=POSNUMBER)?
   | TEXT (STAR maxlength=POSNUMBER)?
   | NAME_CONST
   | URI
   ;

textConst
   : textconst=STRING
   ;

enumerationType
   : enumeration (ORDERED | CIRCULAR)?
   ;

enumTreeValueType
   : ALL OF typeref=path
   ;

alignmentType
   : HALIGNMENT | VALIGNMENT
   ;

booleanType
   : BOOLEAN
   ;

enumeration
   : LPAREN enumElement (COMMA enumElement)* (COLON FINAL)? RPAREN
   | LPAREN FINAL RPAREN
   ;

enumElement
   : NAME (DOT NAME)* (sub=enumeration)?
   ;

enumConst
   : HASH (NAME (DOT NAME)* (DOT OTHERS)?
   | OTHERS)
   ;

numericType
   : (min=decimal DOTDOT max=decimal | NUMERIC ) CIRCULAR?
     (LBRACE unitref=path RBRACE)?
     (CLOCKWISE | COUNTERCLOCKWISE | refSys)?
   ;

dateTimeType
   : (DATE | TIMEOFDAY | DATETIME)
   ;

refSys
   : (LCURLY refsys=metaObjectRef (LBRACE axis=POSNUMBER RBRACE)? RCURLY
   | LESS coord=path (LBRACE axis=POSNUMBER RBRACE)? GREATER)
   ;

decConst
   : dec=decimal
   | PI 
   | LNBASE
   ;

numericConst
   : decConst (LBRACE unitref=path RBRACE)?
   ;

formattedType
   : FORMAT BASED ON structref=path formatDef (min=STRING DOTDOT max=STRING)?
   | FORMAT formatref=path min=STRING DOTDOT max=STRING
   | min=STRING DOTDOT max=STRING
   ;

formatDef 
   : LPAREN INHERITANCE?
     nonnumeric=STRING? (baseAttrRef nonnumeric=STRING)*
     baseAttrRef nonnumeric=STRING? RPAREN
   ;

baseAttrRef
   : NAME (SLASH POSNUMBER)?
   | NAME SLASH baseattr=path
   ;

formattedConst
   : formattedconst=STRING
   ;

contextDef
   : CONTEXT contextBlock+
   ;

contextBlock
   : name=NAME EQUAL (contextDecl SEMI)*
   ;

contextDecl
   : generic=path EQUAL path (OR path)*
   ;
   
coordinateType
   : (COORD | {ili24}?MULTICOORD) numtype1=numericType (COMMA numtype2=numericType (COMMA numtype3=numericType)?)? (COMMA rotationDef)?
   ;
   
rotationDef
   : ROTATION nullaxis=POSNUMBER RARROW pihalfaxis=POSNUMBER
   ;

oIDType
   : OID (ANY | numericType | textType)
   ;

blackboxType
   : BLACKBOX (XML | BINARY)
   ;

classRefType
   : CLASS restriction?
   | STRUCTURE restriction?
   ;

attributePathType
   : ATTRIBUTE (OF (attributePath | AT NAME))?
     (RESTRICTION LPAREN attrTypeDef (COLON attrTypeDef)* RPAREN)? 
   ;

classConst
   : GREATER classref=path
   ;

attributePathConst
   : GREATERGREATER (classref=path RARROW)? attribute=NAME
   ;

lineType
   : (directed=DIRECTED? POLYLINE|SURFACE|AREA|({ili24}? (multdir=DIRECTED? MULTIPOLYLINE) | MULTISURFACE | MULTIAREA))
     lineForm? 
     (VERTEX coordref=path)? 
     (WITHOUT OVERLAPS GREATER overlap=decimal | {ili24}? WITHOUT OVERLAPS)?
     ({ili23}? LINE ATTRIBUTES lineattrstruct=path)?
   ;

lineForm 
   : WITH LPAREN lineFormType (COMMA lineFormType)* RPAREN
   ;
   
lineFormType
   : STRAIGHTS
   | ARCS
   | path
   ;

lineFormTypeDef
   : LINE FORM (lineFormTypeDecl)*
   ;
   
lineFormTypeDecl
   : lineformname=NAME COLON structureref=path SEMI
   ;
   
unitDecl
   : UNIT (unitDef)*
   ;

unitDef
   : unitname=NAME 
     (LPAREN ABSTRACT RPAREN | LBRACE unitshort=path RBRACE)?
     (EXTENDS super=path)?
     (EQUAL (derivedUnit | composedUnit))? 
     SEMI 
   ;

derivedUnit 
   : (decConst (op=(STAR | SLASH) decConst)* | FUNCTION EXPLANATION)? 
     LBRACE unitref=path RBRACE
   ;

composedUnit
   : LPAREN composedUnitExpr RPAREN
   ;

composedUnitExpr
   : path
   | composedUnitExpr STAR path
   | composedUnitExpr SLASH path
   ;

metaDataBasketDef
   : (SIGN | REFSYSTEM) BASKET basketname=NAME
     (LPAREN FINAL RPAREN)?
     (EXTENDS metaDataBasketRef)?
     TILDE path 
     (OBJECTS OF classname=NAME COLON metabjectname=NAME 
                         ( COMMA metaobjectname=NAME)*)* SEMI
   ;

metaDataBasketRef
   : ((modelname=NAME DOT)? topicname=NAME DOT)? basketname=NAME
   ;

metaObjectRef
   : (metaDataBasketRef DOT)? metaobjectname=NAME
   ;

parameterDef 
   : parameterName=NAME
     properties? // ABSTRACT,EXTENDED,FINAL
     COLON (attrTypeDef | METAOBJECT (OF path)?) SEMI
   ;

properties
   : LPAREN 
     (ABSTRACT|EXTENDED|FINAL|TRANSIENT|OID|HIDING|ORDERED|EXTERNAL|GENERIC) 
     (COMMA (ABSTRACT|EXTENDED|FINAL|TRANSIENT|OID|HIDING|ORDERED|EXTERNAL|GENERIC))* 
     RPAREN
   ;

runTimeParameterDef
   : PARAMETER runTimeParameter*
   ;

runTimeParameter
   : runtimeparametername=NAME COLON attrTypeDef SEMI
   ;

constraintDef
   : mandatoryConstraint
   | plausibilityConstraint
   | existenceConstraint
   | uniquenessConstraint
   | setConstraint 
   ;

mandatoryConstraint
   : MANDATORY CONSTRAINT ({ili24}? name=NAME COLON)? booleanexp=expression SEMI
   ;

plausibilityConstraint
   : CONSTRAINT ({ili24}? name=NAME COLON)? 
     (LESSEQUAL | GREATEREQUAL) percentage=decimal PERCENT
     expression SEMI
   ;

existenceConstraint
   : EXISTENCE CONSTRAINT ({ili24}? name=NAME COLON)? attributePath REQUIRED IN
     path COLON attributePath
     (OR path COLON attributePath )* SEMI
   ;

uniquenessConstraint
   : UNIQUE ({ili24}? LPAREN BASKET RPAREN)?
     ({ili24}? name=NAME COLON)?
     (WHERE expression COLON)?
     (globalUniqueness | localUniqueness) SEMI
   ;

globalUniqueness
   : uniqueEl
   ;

uniqueEl
   : objectOrAttributePath (COMMA objectOrAttributePath)*
   ;

localUniqueness
   : LPAREN LOCAL RPAREN localUniqueEl
   ;
   
localUniqueEl
   : structureattributename=NAME
     (RARROW structureattributename=NAME)* COLON
     attributename=NAME (COMMA attributename=NAME)*
   ;

setConstraint
   : SET CONSTRAINT ({ili24}? LPAREN BASKET RPAREN)?
     ({ili24}? name=NAME COLON)?
     (WHERE logical=expression COLON)? expression SEMI
   ;

constraintsDef
   : CONSTRAINTS OF path EQUAL
     (constraintDef)*
     END SEMI
   ;

expression
   : term1
   ;

term1
   : term2 (operator1 term2)*
   ;
   
operator1
   : OR
   | {ili24}? PLUS
   | {ili24}? MINUS
   | {ili24}? IMPL
   ;

term2
   : term3 (operator2 term3)*
   ;
   
operator2
   : AND
   | {ili24}? STAR
   | {ili24}? SLASH
   ;

term3
   : t1=term (relation t2=term)?
   ;

term
   : factor                       
   | (NOT? LPAREN expression RPAREN)
   | (DEFINED LPAREN factor RPAREN)
   ;
   
relation
   : EQUALEQUAL
   | NOTEQUAL
   | LESSGREATER
   | LESSEQUAL
   | GREATEREQUAL
   | LESS
   | GREATER
   ;

factor
   : objectpath=objectOrAttributePath 
   | (inspection | INSPECTION path) (OF inspaectionpath=objectOrAttributePath)? 
   | functionCall
   | PARAMETER parampath=path
   | constant 
   ;

objectOrAttributePath
   : pathEl (RARROW pathEl)*
   ;

attributePath
   : objectOrAttributePath
   ;

pathEl
   : THIS
   | THISAREA 
   | THATAREA
   | PARENT
   | objectRef 
   ;

objectRef 
   : (BACKSLASH)? (name=NAME (LBRACE (FIRST | LAST | axislistindex=POSNUMBER | associationname=NAME) RBRACE)? | AGGREGATES)
   ;

functionCall
   : functionname=path 
     LPAREN (functionCallArgument (COMMA functionCallArgument)*)? RPAREN
   ;

functionCallArgument
   : expression
     | ALL (LPAREN restrictedRef | path RPAREN)*
   ;

functionDef
   : FUNCTION functioname=NAME
     LPAREN (functionDefParam (SEMI functionDefParam)*)? RPAREN
     COLON result=argumentType 
     EXPLANATION? 
     SEMI
   ;

functionDefParam
   : argumentname=NAME COLON argumentType
   ;
 
argumentType
   : attrTypeDef 
   | (OBJECT | OBJECTS) OF (restrictedRef | viewref=path)
   | ENUMVAL 
	| ENUMTREEVAL
   ;

viewDef
   : VIEW viewname1=NAME
     properties? // ABSTRACT|EXTENDED|FINAL|TRANSIENT
     (formationDef? | EXTENDS viewref=path)
     (baseExtensionDef)*
     (selection)*
     EQUAL
     ATTRIBUTE?
     viewAttribute*
     constraintDef*
     END viewname2=NAME SEMI
   ;

formationDef
   : (projection | join | iliunion  | aggregation | inspection) SEMI
   ;

projection
   : PROJECTION OF renamedViewableRef
   ;

join
   : JOIN OF renamedViewableRef
     (COMMA renamedViewableRef (LPAREN OR ILINULL RPAREN)?)+
   ;

iliunion
   : UNION OF renamedViewableRef
     (COMMA renamedViewableRef)*
   ;

aggregation
   : AGGREGATION OF renamedViewableRef
    (ALL | EQUAL_CONST LPAREN uniqueEl RPAREN)
   ;

inspection
   : AREA? INSPECTION OF renamedViewableRef
     (RARROW structureorlineattributename=NAME)+
   ;

renamedViewableRef
   : (basename=NAME TILDE)? path
   ;

baseExtensionDef
   : BASE basename=NAME EXTENDED BY
     renamedViewableRef (COMMA renamedViewableRef)*
   ;

selection
   : WHERE expression SEMI
   ;

viewAttribute
   : ALL OF basename=NAME SEMI
   | attributeDef
   | attributename=NAME properties? // ABSTRACT|EXTENDED|FINAL|TRANSIENT
        COLONEQUAL factor SEMI 
   ;

graphicDef
   : GRAPHIC graphicname1=NAME properties? // ABSTRACT|FINAL
     (EXTENDS expath=path)?
     (BASED ON bpath=path)? 
     EQUAL
     (selection)*
     (drawingRule)*
     END graphicname2=NAME SEMI
   ;
             
drawingRule
   : drawingrulename=NAME properties? // ABSTRACT|EXTENDED|FINAL
     (OF path)?
     COLON condSignParamAssignment (SEMI condSignParamAssignment)* SEMI
   ;

condSignParamAssignment
   : (WHERE logical=expression)?
     LPAREN signParamAssignment (SEMI signParamAssignment)* RPAREN
   ;

signParamAssignment
   : signparametername=NAME
     COLONEQUAL ( 
       LCURLY metaObjectRef RCURLY
       | factor
       | ACCORDING enumpath=attributePath LPAREN enumAssignment (COMMA enumAssignment)* RPAREN
     )
   ;

enumAssignment
   : (LCURLY metaObjectRef RCURLY | constant) 
     WHEN IN enumRange
   ;

enumRange
   : enumConst (DOTDOT enumConst)?
   ;
