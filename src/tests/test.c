#include <stdlib.h>
#include <stdio.h>
#include <CUnit/Basic.h>
#include "../libfractal/fractal.h"
#include "../fractstack/fractstack.h"
#include "../prodcons/prodcons.h"

/**
 * Tests whether a fractal has the right name after initialising it.
 */
void get_name(void)
{
	const char *name = "get_name";
	int width = 1000;
	int height = 1000;
	double a = -0.8;
	double b = 0.156;
	struct fractal *f = fractal_new(name, width, height, a, b);
	CU_ASSERT_STRING_EQUAL(fractal_get_name(f), name);
	fractal_free(f);
}


/**
 * Tests whether a fractal has the right width and height after initialising it.
 */
void get_width_and_height(void)
{
	const char *name = "get_width_and_height";
	int width = 900;
	int height = 900;
	double a = 0.8;
	double b = 0.1;
	struct fractal *f = fractal_new(name, width, height, a, b);
	CU_ASSERT_EQUAL(fractal_get_width(f), width);
	CU_ASSERT_EQUAL(fractal_get_height(f), height);
	fractal_free(f);
}



/**
 * Tests whether a fractal has the right values after setting and getting them.
 */
void set_and_get_value(void)
{
	const char *name = "set_and_get_value";
	int width = 1000;
	int height = 1000;
	double a = -0.8;
	double b = 0.156;
	struct fractal *f = fractal_new(name, width, height, a, b);
	int x = 420;
	int y = 420;
	int value = 69;
	fractal_set_value(f, x, y, value);
	CU_ASSERT_EQUAL(fractal_get_value(f, x, y), value);
	fractal_free(f);
}

/**
 * Tests whether the a value is retrieved correctly.
 */
void get_a(void) {
	const char *name = "get_a";
	int width = 4;
	int height = 2;
	double a = 0.5;
	double b = 0.4;
	struct fractal *f = fractal_new(name, width, height, a, b);
	CU_ASSERT_DOUBLE_EQUAL(fractal_get_a(f), a, 0);
	fractal_free(f);
}

void get_b(void)
{
	const char *name = "get_b";
	int width = 4;
	int height = 2;
	double a = 0.5;
	double b = 0.4;
	struct fractal *f = fractal_new(name, width, height, a, b);
	CU_ASSERT_DOUBLE_EQUAL(fractal_get_b(f), b, 0);
	fractal_free(f);
}

/**
 * Test whether the line_to_fractal function is correctly implemented.
 */
void line_fractal(void)
{
	const char *name = "line_fractal";
	int width = 1000;
	int height = 1000;
	double a = -0.8;
	double b = 0.156;
	char *line = "line_fractal 1000 1000 -0.8 0.156";
	struct fractal *f = line_to_fractal(line);
	CU_ASSERT_STRING_EQUAL(fractal_get_name(f), name);
	CU_ASSERT_EQUAL(fractal_get_width(f), width);
	CU_ASSERT_EQUAL(fractal_get_height(f), height);
	CU_ASSERT_DOUBLE_EQUAL(fractal_get_a(f), a, 0);
	CU_ASSERT_DOUBLE_EQUAL(fractal_get_b(f), b, 0);
	fractal_free(f);
}

int main(int argc, const char *argv[]) {
	CU_pSuite pSuite = NULL;
	// Initialise Suite.
	if (CUE_SUCCESS != CU_initialize_registry())
	{
		return CU_get_error();
	}

	// Create Suite.
	pSuite = CU_add_suite("Suite", NULL, NULL);
	if (NULL == pSuite)
	{
		CU_cleanup_registry();
		return CU_get_error();
	}

	// Add to Suite.
	if (NULL == CU_add_test(pSuite, "get_name", get_name) ||
	   NULL == CU_add_test(pSuite, "set_and_get_value", set_and_get_value) ||
	   NULL == CU_add_test(pSuite, "get_width_and_height", get_width_and_height) || NULL == CU_add_test(pSuite, "get_a", get_a) || NULL == CU_add_test(pSuite, "get_b", get_b) || NULL == CU_add_test(pSuite, "line_fractal", line_fractal))
    {
		CU_cleanup_registry();
		return CU_get_error();
	}

	// Launch tests.
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();
}
