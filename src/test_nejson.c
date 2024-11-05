/*
#define STB_LEAKCHECK_IMPLEMENTATION
#include <stb_leakcheck.h>

#define NEJSON_MALLOC(_size) malloc(_size)
#define NEJSON_REALLOC(_memory, _new_size) realloc(_memory, _new_size)
#define NEJSON_FREE(_memory) free(_memory)
 */

#define NEJSON_IMPLEMENTATION
#include <nejson.h>
#include <locale.h>

int main()
{
   struct nejson json;

   setlocale(LC_ALL, "");
   
   nejson__init(&json);

   nejson__load_from_file(
       &json,
       "resources/json.json");

   printf("String: %s\n",
          nejson__get_string_or_undefined_tree(
              &json,
              "s", "string"));

   nejson__set_string_tree(
       &json,
       "TRUE!",
       "s",
       "Adachi say");

   nejson__set_string_utf16_tree(
       &json,
       L"ЦЕ ТАК!",
       "w",
       L"Адачі каже");

   nejson__save_to_file(
       &json,
       "resources/json_saved.json", 1);

   nejson__free(&json);

   /*
   stb_leakcheck_dumpmem();
   */
   return 0;
}
