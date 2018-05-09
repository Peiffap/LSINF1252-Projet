#include <stdlib.h>
#include <stdio.h>
#include <CUnit/Basic.h>
#include "../libfractal/fractal.h"
#include "../fractstack/fractstack.h"
#include "../prodcons/prodcons.h"

/**
 * Tests whether a fractal has the right name after initialising it.
 */
void getName(void)
{
	const char *name = "nametest";
	int width = 1000;
	int height = 1000;
	double a = -0.8;
	double b = 0.156;
	struct fractal *f = fractal_new(name, width, height, a, b);
	CU_ASSERT_STRING_EQUAL(fractal_get_name(f), name);
	fractal_free(f);
}

/**
 * Tests whether a fractal has the right values after setting and getting them.
 */
void setAndGetValue(void)
{
	const char *name = "setandgettest";
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
	if (NULL == CU_add_test(pSuite, "GetName", getName) ||
	   NULL == CU_add_test(pSuite, "SetAndGetValue", setAndGetValue))
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
