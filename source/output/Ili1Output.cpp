#pragma once

#include "Ili1Output.h"
#include "../metamodel/MetaModelOutput.h"
#include "../util/TextWriter.h"
#include "../util/Logger.h"

using namespace util;
using namespace metamodel;
using namespace output;

// public

Ili1Output::Ili1Output(metamodel::MetaModelStore &store,util::Logger &logger,string ili_file)
   : MetaModelTreeVisitor(store,logger)
{
   this->ili_file = ili_file;
}

void Ili1Output::preVisitModel(Model* m)
{
    logger().warning("INTERLIS 1 generation is not fully implemented yet");

    ili1.openFile(ili_file);
    ili1.writeln("TRANSFER INTERLIS1;");

    ili1.writeln("");
    ili1.writeln("MODEL " + m->Name);

    ili1.incNestLevel();

    domainheader_written = false;
}

void Ili1Output::postVisitModel(Model* m)
{
    ili1.decNestLevel();
    ili1.writeln("");
    ili1.writeln("END " + m->Name + ".");

    ili1.writeln("");
    ili1.writeln("FORMAT FREE;");

    //ili1.writeln("FORMAT FIX WITH LINESIZE = 110, TIDSIZE = 16; ");

    ili1.writeln("CODE");
    ili1.incNestLevel();
    ili1.writeln("BLANK = DEFAULT, UNDEFINED = DEFAULT, CONTINUE = DEFAULT;");
    ili1.writeln("TID = ANY;");
    ili1.decNestLevel();
    ili1.writeln("END.");

    ili1.closeFile();

    domainheader_written = false;
}

void Ili1Output::preVisitSubModel(SubModel* s)
{
    ili1.writeln("");
    ili1.writeln("TOPIC " + s->Name + " =");
    ili1.incNestLevel();

    domainheader_written = false;
}

void Ili1Output::postVisitSubModel(SubModel* s)
{
    ili1.decNestLevel();
    ili1.writeln("");
    ili1.writeln("END " + s->Name + ".");

    domainheader_written = false;
}

// a fix to avoid structure written inside a class
// pre/postVisitClass is called also for structures as an attribute type 
// within a class

void Ili1Output::preVisitClass(Class* c)
{
    if (c->Kind == Class::ViewVal) {
        return;
    }
    else if (c->Kind == Class::Structure) {
        if (visitClassFlag) {
            return;
        }
    }
    else {
        visitClassFlag = true;
    }


    // ClassVal, Structure

    ili1.writeln("");
    ili1.writeln("TABLE " + c->Name + " =");
    ili1.incNestLevel();

}

void Ili1Output::postVisitClass(Class* c)
{
    if (c->Kind == Class::ViewVal) {
        return;
    }
    else if (c->Kind == Class::Structure) {
        if (visitClassFlag) {
            return;
        }
    }
    else {
        visitClassFlag = false;
    }

    // ClassVal, Structure

    ili1.decNestLevel();
    ili1.writeln("NO IDENT");
    ili1.writeln("END " + c->Name + ";");

}


void Ili1Output::visitAttrOrParam(AttrOrParam *a)
{
   if (a->AttrParent == nullptr) {
      return;
   }

   if (a->AttrParent->Kind == Class::ViewVal) {
      return;
   }

   // ClassVal, Structure

   if (a->Type != nullptr) {

       // structure
       if (a->Type->getClass() == "Class") {
  
           TextType* tt = static_cast<TextType*>(a->Type);

           string type = "";

           if (tt->Mandatory == false) {
                type = type + "OPTIONAL ";
           }
           type = type + "-> " + a->Type->Name;

           ili1.writeln(a->Name + ": " + type + ";");

       }
       // normal type
       else {
           DomainType* t = dynamic_cast<DomainType*>(a->Type);

           string type = get_type(t, false);

           logger().message(a->AttrParent->Name + "." + a->Name + ":" + type);

           if (type.size() > 0) {
               ili1.writeln(a->Name + ": " + type + ";");
           }
           else {
               ili1.writeln("!! " + a->Name + ": Type=" + t->getClass() + " not supported");
           }
       }
   }
   else {
        ili1.writeln("!! " + a->Name + ": Type=" + "unknown" + " not supported");
   }

}

void Ili1Output::visitRole(Role* r)
{
    string strongness;
    if (r->Strongness == Role::Assoc) {
        strongness = "Association";
    }
    else if (r->Strongness == Role::Aggr) {
        strongness = "Aggregation";
    }
    else { // Role::Comp
        strongness = "Compoisition";
    }

    string multiplicity;
    if (r->Multiplicity.Min == r->Multiplicity.Max) {
        multiplicity = to_string(r->Multiplicity.Min);
    }
    else if (r->Multiplicity.Max == -1) {
        multiplicity = to_string(r->Multiplicity.Min) + "..*";
    }
    else {
        multiplicity = to_string(r->Multiplicity.Min) + ".." + to_string(r->Multiplicity.Max);
    }

    string targets = r->_baseclass->Name;
    string targets_additional = "";
    for (auto tt : r->_classrestriction) {
       if (targets_additional == "") {
          targets_additional = tt->Name;
       }
       else {
          targets_additional += ", " + tt->Name;
       }
    }

    string mandatory = "";
    if (r->Mandatory = false) {
        mandatory = " OPTIONAL ";
    }

    string remark = " !! " + strongness + ": {" + multiplicity + "}";
    if (targets_additional.size() > 0) {
        remark = remark + " additional targets: " + targets_additional;
    }

    if (targets.size() <= 0) {
        logger().warning(r->Association->Name + "." + r->Name + " targets unknown");

        targets = "targets unknown";
        ili1.writeln("!! " + r->Name + ": " + mandatory + "-> " + targets + ";" + remark);
    }
    else {
        ili1.writeln(r->Name + ": " + mandatory + "-> " + targets + ";" + remark);
    }

}

void Ili1Output::visitDomainType(metamodel::DomainType* t)
{
    if (t->_attr != nullptr) {
    }
    else if (t->Name == "C1") {
    }
    else if (t->Name == "C2") {
    }
    else if (t->Name == "C3") {
    }
    else if (t->Name == "A1") {
    }
    else if (t->Name == "A2") {
    }
    else if (t->Name == "R") {
    }
    else if (t->Name == "TOP") {
    }
    else if (t->Name == "") {
    }
    else if (t->Name == "TYPE") {
    }
    else {

        // domain header
        if (!domainheader_written) {
                domainheader_written = true;
                ili1.writeln("");
                ili1.writeln("DOMAIN");
                ili1.writeln("");
        }

            // domain

        ili1.incNestLevel();
        string type = get_type(t, true);
        ili1.writeln(t->Name + " = " + type + ";");
        ili1.decNestLevel();
    }

}

// private

string Ili1Output::get_enum_nodes(list <EnumNode*> ln)
{
    string nodes = "";

    if (ln.size() <= 0) return nodes;

    nodes = "(\n";

    ili1.incNestLevel();

    bool comma = false;
    for (EnumNode* n : ln) {

        if (comma) {
            nodes = nodes + ",\n";
        }
        comma = true;

        for (int i = 0; i < ili1.getNestLevel(); i++) {
            nodes = nodes + "   ";
        }

        nodes = nodes + n->Name;

        string subnodes = get_enum_nodes(n->Node);
        if (subnodes.size() > 0) {
            nodes = nodes + subnodes;
        }
    }

    ili1.decNestLevel();

    nodes = nodes + ")";

    return nodes;
}

string Ili1Output::get_type(DomainType* t, bool domainflag)
{
    string type = "";

    /*
    logger().message("get_type:");
    if (t->ElementInPackage != nullptr) {
        logger().message("t->ElementInPackage->Name=" + t->ElementInPackage->Name);
    };
    logger().message("t->Name=" + t->Name);
    */

    // if (t->ElementInPackage != nullptr) {
    //    if (t->ElementInPackage->Name == "INTERLIS") {
            if ((t->Name == "HALIGNMENT") ||
                (t->Name == "VALIGNMENT")) {
                type = t->Name;
            }
            else if (t->Name == "INTERLIS_1_DATE") {
                type = "DATE";
            }
    //    }
    // }
    

    if (type.size() <= 0) {
        if (t->getClass() == "TextType") {
            TextType* tt = static_cast<TextType*>(t);

            int tl = tt->MaxLength;
            if (tl <= 0) tl = 2000;

            type = type + "TEXT*" + to_string(tl);
        }

        else if (t->getClass() == "NumType") {
            NumType* tt = static_cast<NumType*>(t);

            type = type + " [" + tt->Min + " .. " + tt->Max + ']';
        }

        else if (t->getClass() == "EnumType") {
            EnumType* tt = static_cast<EnumType*>(t);

            string nodes = get_enum_nodes(tt->TopNode->Node);

            if (nodes.size() > 0) {
                type = type + nodes;
            }
        }

        else if (t->getClass() == "CoordType") {
            CoordType* tt = static_cast<CoordType*>(t);
            type = type + "COORD" + to_string(tt->Axis.size());
            for (auto a : tt->Axis) {
                type = type + " " + a->Min + " " + a->Max;
            }
        }

        else if (t->getClass() == "LineType") {
            LineType* tt = static_cast<LineType*>(t);

            switch (tt->Kind) {
            case LineType::Polyline:
            case LineType::DirectedPolyline:
                type = "POLYLINE";
                break;
            case LineType::Surface:
                type = "SURFACE";
                break;
            case LineType::Area:
                type = "AREA";
                break;
            }

            type = type + " WITH (STRAIGHTS)";

            if (tt->CoordType != nullptr) {
                type = type + " VERTEX " + tt->CoordType->Name;
            }
            else {
                type = type + " VERTEX " + "COORD2 0.000 0.000 0.000 0.000";
            }
            if (tt->MaxOverlap != "") {
                type = type + " WITHOUT OVERLAPS > " + tt->MaxOverlap;
            }
            if (tt->LAStructure != nullptr) {
                // to do
            }
        }
        else {
            logger().warning("unknown type: " + t->getClass());
        }
    }

    if (type.size() > 0) {
        if (t->Mandatory == false) {
            if (!domainflag) {
                type = "OPTIONAL " + type;
            }
        }
    }

    return type;
}
