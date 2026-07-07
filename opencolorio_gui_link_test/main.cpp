#include <CtrlLib/CtrlLib.h>
#include <Ui/Ui.h>

#include <exception>
#include <cstdio>
#include <cstring>

#include <opencolorio/OpenColorIO.h>

#ifndef UPP_IMAGING_LOCAL_OPENCOLORIO_INCLUDE
#error local OpenColorIO package not selected
#endif

namespace OCIO = OCIO_NAMESPACE;

using namespace Upp;

static OCIO::GroupTransformRcPtr MakeProgrammaticGroup()
{
	auto group = OCIO::GroupTransform::Create();
	auto matrix = OCIO::MatrixTransform::Create();
	double m44[16] = {
		2.0, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 1.5, 0.0,
		0.0, 0.0, 0.0, 1.0,
	};
	double offset[4] = {0.1, 0.2, -0.1, 0.0};
	matrix->setMatrix(m44);
	matrix->setOffset(offset);
	group->appendTransform(matrix);
	return group;
}

GUI_APP_MAIN
{
	try {
		int passed = 0;
		int failed = 0;

		OCIO::ConstConfigRcPtr config = OCIO::Config::Create();
		OCIO::ConstProcessorRcPtr processor = config ? config->getProcessor(MakeProgrammaticGroup()) : OCIO::ConstProcessorRcPtr();
		OCIO::ConstCPUProcessorRcPtr cpu = processor ? processor->getDefaultCPUProcessor() : OCIO::ConstCPUProcessorRcPtr();
		OCIO::ConstGPUProcessorRcPtr gpu = processor ? processor->getDefaultGPUProcessor() : OCIO::ConstGPUProcessorRcPtr();
		OCIO::GpuShaderDescRcPtr desc = OCIO::GpuShaderDesc::CreateShaderDesc();

		if(config && processor) {
			printf("OCIO GUI core link: OK\n");
			passed++;
		} else {
			printf("OCIO GUI core link: FAIL\n");
			failed++;
		}

		if(cpu) {
			float pixel[4] = {0.2f, 0.4f, 0.6f, 0.7f};
			cpu->applyRGBA(pixel);
			printf("OCIO GUI CPU processor: OK\n");
			passed++;
		} else {
			printf("OCIO GUI CPU processor: FAIL\n");
			failed++;
		}

		if(gpu && desc) {
			desc->setLanguage(OCIO::GPU_LANGUAGE_GLSL_4_0);
			desc->setFunctionName("OCIOGui");
			desc->setResourcePrefix("ocio_");
			gpu->extractGpuShaderInfo(desc);
			if(desc->getShaderText() && *desc->getShaderText()) {
				printf("OCIO GUI GPU processor: OK\n");
				printf("OCIO GUI GLSL extraction: OK\n");
				passed += 2;
			} else {
				printf("OCIO GUI GPU processor: FAIL\n");
				printf("OCIO GUI GLSL extraction: FAIL\n");
				failed += 2;
			}
		} else {
			printf("OCIO GUI GPU processor: FAIL\n");
			printf("OCIO GUI GLSL extraction: FAIL\n");
			failed += 2;
		}

		printf("SUMMARY passed=%d failed=%d\n", passed, failed);
		SetExitCode(failed ? 1 : 0);
	} catch(const OCIO::Exception& e) {
		printf("OCIO exception: %s\n", e.what());
		SetExitCode(1);
	} catch(const std::exception& e) {
		printf("std::exception: %s\n", e.what());
		SetExitCode(1);
	}
}
