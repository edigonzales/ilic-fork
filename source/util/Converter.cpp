#include <cstring>
#include <string>

#include "Converter.h"

string util::to_string_bool(bool value)
{
   if (value) {
      return "true";
   }
   else {
      return "false";
   }
}

string util::ansi_date_to_ymd(char date[])
{

   // convert ANSI C date to year-month-day

   unsigned int d, m, y;
   string day, month, year;
   const char *months[] = {
      "Jan", "Feb", "Mar", "Apr", "May", "Jun",
      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
   };
   char temp[20];
   strcpy(temp, date);

   // year
   y = atoi(temp + 9);
   *(temp + 6) = 0;
   year = "20" + to_string(y);

   // day
   d = atoi(temp + 4);
   *(temp + 3) = 0;
   if (d < 10) {
      day = "0" + to_string(d);
   }
   else {
      day = to_string(d);
   }

   // month
   for (m = 1; m <= 12; m++) {
      if (strcmp(temp, months[m - 1]) == 0) {
         break;
      }
   }
   if (m < 10) {
      month = "0" + to_string(m);
   }
   else {
      month = to_string(m);
   }

   return year + "-" + month + "-" + day;

}
