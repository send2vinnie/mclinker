/*****************************************************************************
 *   Test Suite of The BOLD Project,                                         *
 *                                                                           *
 *   Copyright (C), 2011 -                                                   *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   ${AUTHOR} <${EMAIL}>                                                    *
 ****************************************************************************/
#include <${class_name}.h>
#include <${class_name}Test.h>

using namespace BOLD;
using namespace BOLDTEST;


// Constructor can do set-up work for all test here.
${class_name}Test::${class_name}Test()
{
	// create testee. modify it if need
	m_pTestee = new ${class_name}();
}

// Destructor can do clean-up work that doesn't throw exceptions here.
${class_name}Test::~${class_name}Test()
{
	delete m_pTestee;
}

// SetUp() will be called immediately before each test.
void ${class_name}Test::SetUp()
{
}

// TearDown() will be called immediately after each test.
void ${class_name}Test::TearDown()
{
}

