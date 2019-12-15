#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main ( int argc, char **argv ) {
	if ( argc < 3 ) return -1;
	char *str_class_name = argv[1];
	char *str_base_class = argv[2];

	int length = strlen ( str_class_name );
	int type = 0;

	char *s = str_class_name;
	for ( int i = 1; i < length; i++ ) {
		if ( s[i] >= 'A' && s[i] <= 'Z' ) {
			type = i;
		}
	}

	char *class_name = strdup ( str_class_name );
	char *base_class = strdup ( str_base_class );

	char *gobject_prefix = calloc ( type + 1, 1 );
	for ( int i = 0; i < type; i++ ) {
		gobject_prefix[i] = class_name[i] >= 'a' && class_name[i] <= 'z' ? class_name[i] - 32 : class_name[i];
	}
	int len = length - type;
	char *gobject_type = calloc ( len + 1, 1 );
	for ( int i = type, index = 0; i < length; i++, index++ ) {
		gobject_type[index] = class_name[i] >= 'a' && class_name[i] <= 'z' ? class_name[i] - 32 : class_name[i];
	}
	char *class_prefix = calloc ( length + 2, 1 );
	for ( int i = 0; i < type; i++ ) {
		class_prefix[i] = gobject_prefix[i] >= 'A' && gobject_prefix[i] <= 'Z' ? gobject_prefix[i] + 32 : gobject_prefix[i];
	}
	class_prefix[type] = '_';
	for ( int i = type + 1, index = 0; i < length + 1; i++, index++ ) {
		class_prefix[i] = gobject_type[index] >= 'A' && gobject_type[index] <= 'Z' ? gobject_type[index] + 32 : gobject_type[index];
	}

	printf ( "prefix: %s\n", gobject_prefix );
	printf ( "type: %s\n", gobject_type );
	printf ( "function: %s\n", class_prefix );

	char *source = calloc ( 255, 1 );
	char *header = calloc ( 255, 1 );
	snprintf ( source, 254, "%s.c", class_prefix );
	snprintf ( header, 254, "%s.h", class_prefix );

	char *str_header = calloc ( 4096, 1 );
	snprintf ( str_header, 4095,
			"#ifndef _%s_%s_H_\n"
			"#define _%s_%s_H_\n"
			"\n"
			"#include <glib-object.h>\n"
			"#include <gtk/gtk.h>\n"
			"\n"
			"G_BEGIN_DECLS\n"
			"\n"
			"#define %s_TYPE_%s (%s_get_type())\n"
			"#define %s_%s(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), %s_TYPE_%s, %s))\n"
			"#define %s_%s_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), %s_TYPE_%s, %sClass))\n"
			"#define %s_IS_%s(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), %s_TYPE_%s))\n"
			"#define %s_IS_%s_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), %s_TYPE_%s))\n"
			"#define %s_%s_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), %s_TYPE_%s, %sClass))\n"
			"\n"
			"typedef struct _%sClass %sClass;\n"
			"typedef struct _%s %s;\n"
			"typedef struct _%sPrivate %sPrivate;\n"
			"\n\n"
			"struct _%sClass\n"
			"{\n"
			"  %sClass parent_class;\n"
			"};\n"
			"\n"
			"struct _%s\n"
			"{\n"
			"  %s parent_instance;\n"
			"  %sPrivate *priv;\n"
			"};\n"
			"\n"
			"GType %s_get_type (void) G_GNUC_CONST;\n"
			"\n"
			"G_END_DECLS\n"
			"\n"
			"#endif  /* _%s_%s_H_ */\n"
			,
		gobject_prefix, gobject_type,
		gobject_prefix, gobject_type,
		gobject_prefix, gobject_type, class_prefix,
		gobject_prefix, gobject_type, gobject_prefix, gobject_type, class_name,
		gobject_prefix, gobject_type, gobject_prefix, gobject_type, class_name,
		gobject_prefix, gobject_type, gobject_prefix, gobject_type,
		gobject_prefix, gobject_type, gobject_prefix, gobject_type,
		gobject_prefix, gobject_type, gobject_prefix, gobject_type, class_name,
		class_name, class_name,
		class_name, class_name,
		class_name, class_name,
		class_name,
		base_class,
		class_name,
		base_class,
		class_name,
		class_prefix,
		gobject_prefix, gobject_type

		);

	FILE *fp_header = fopen ( header, "w" );
	fwrite ( str_header, 1, strlen ( str_header ), fp_header );
	fclose ( fp_header );

	char *str_source = calloc ( 4096, 1 );
	snprintf ( str_source, 4095,
			"#include \"%s.h\"\n"
			"\n"
			"struct _%sPrivate\n"
			"{\n"
			"};\n"
			"\n"
			"\n"
			"G_DEFINE_TYPE (%s, %s, G_TYPE_OBJECT);\n"
			"\n"
			"static void %s_init ( %s *%s ) {\n"
			"  %s->priv = G_TYPE_INSTANCE_GET_PRIVATE ( %s, %s_TYPE_%s, %sPrivate );\n"
			"\n"
			"  /* TODO: Add initialization code here */\n"
			"}\n"
			"\n"
			"static void %s_finalize ( %s *object ) {\n"
			"  /* TODO: Add deinitialization code here */\n"
			"  G_OBJECT_CLASS ( %s_parent_class )->finalize ( object );\n"
			"}\n"
			"\n"
			"static void %s_class_init ( %sClass *klass ) {\n"
			"  %sClass *object_class = GOBJECT_CLASS ( klass );\n"
			"  g_type_class_add_private ( klass, sizeof ( %sPrivate ) );\n"
			"  object_class->finalize = %s_finalize;\n"
			"}\n"
			,
		class_prefix,
		class_name,
		class_name, class_prefix,
		class_prefix, class_name, class_prefix,
		class_prefix, class_prefix, gobject_prefix, gobject_type, class_name,
		class_prefix, base_class,
		class_prefix,
		class_prefix, class_name,
		base_class,
		class_name,
		class_prefix
			);

	FILE *fp_source = fopen ( source, "w" );
	fwrite ( str_source, 1, strlen ( str_source ), fp_source );
	fclose ( fp_source );

}
