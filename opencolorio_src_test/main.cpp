#include <Core/Core.h>

#include <cmath>
#include <exception>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <string>

#include <opencolorio_src/OCIO.h>

#ifndef UPP_IMAGING_LOCAL_OPENCOLORIO_SOURCE_INCLUDE
#error local OpenColorIO source package not selected
#endif

namespace OCIO = OCIO_NAMESPACE;

using namespace Upp;

static bool Near(double a, double b, double eps = 1e-6)
{
	return std::fabs(a - b) <= eps;
}

static bool NearPixel(const float* got, const float* exp, int count, double eps = 1e-6)
{
	for(int i = 0; i < count; ++i) {
		if(!Near(got[i], exp[i], eps))
			return false;
	}
	return true;
}

static void PrintPixel(const char* label, const float* pixel)
{
	printf("%s %.6g %.6g %.6g %.6g\n", label, pixel[0], pixel[1], pixel[2], pixel[3]);
}

static bool HasColorSpace(const OCIO::ConstConfigRcPtr& config, const char* name)
{
	for(int i = 0; i < config->getNumColorSpaces(); ++i) {
		const char* current = config->getColorSpaceNameByIndex(i);
		if(current && std::strcmp(current, name) == 0)
			return true;
	}
	return false;
}

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

	auto exponent = OCIO::ExponentTransform::Create();
	double value[4] = {2.0, 2.0, 2.0, 1.0};
	exponent->setValue(value);
	group->appendTransform(exponent);

	auto range = OCIO::RangeTransform::Create();
	range->setStyle(OCIO::RANGE_NO_CLAMP);
	range->setMinInValue(0.0);
	range->setMaxInValue(1.0);
	range->setMinOutValue(0.0);
	range->setMaxOutValue(1.0);
	group->appendTransform(range);

	return group;
}

static OCIO::ExposureContrastTransformRcPtr MakeDynamicExposureTransform()
{
	auto ec = OCIO::ExposureContrastTransform::Create();
	ec->setStyle(OCIO::EXPOSURE_CONTRAST_LINEAR);
	ec->setExposure(0.0);
	ec->setContrast(1.0);
	ec->setGamma(1.0);
	ec->setPivot(0.18);
	ec->makeExposureDynamic();
	return ec;
}

static bool TestVersion()
{
	const char* version = OCIO::GetVersion();
	return OCIO_VERSION_MAJOR == 2 && OCIO_VERSION_MINOR == 5
		&& OCIO_VERSION_HEX == 0x02050200
		&& version && std::strncmp(version, "2.5.2", 5) == 0;
}

static bool TestCreation()
{
	OCIO::ConstConfigRcPtr config = OCIO::Config::Create();
	OCIO::MatrixTransformRcPtr matrix = OCIO::MatrixTransform::Create();
	if(!config || !matrix)
		return false;

	OCIO::ConstProcessorRcPtr processor = config->getProcessor(matrix);
	if(!processor)
		return false;

	OCIO::ConstCPUProcessorRcPtr cpu = processor->getDefaultCPUProcessor();
	OCIO::ConstGPUProcessorRcPtr gpu = processor->getDefaultGPUProcessor();
	OCIO::GpuShaderDescRcPtr desc = OCIO::GpuShaderDesc::CreateShaderDesc();
	return cpu && gpu && desc && processor->getCacheID() && *processor->getCacheID()
		&& gpu->getCacheID() && *gpu->getCacheID();
}

static bool TestProgrammaticCPU()
{
	OCIO::ConstConfigRcPtr config = OCIO::Config::Create();
	OCIO::ConstProcessorRcPtr processor = config->getProcessor(MakeProgrammaticGroup());
	if(!processor)
		return false;
	OCIO::ConstCPUProcessorRcPtr cpu = processor->getDefaultCPUProcessor();
	if(!cpu)
		return false;

	float pixel1[4] = {0.2f, 0.4f, 0.6f, 0.7f};
	cpu->applyRGBA(pixel1);
	const float expect1[4] = {0.25f, 0.16f, 0.64f, 0.7f};
	if(!NearPixel(pixel1, expect1, 4))
		return false;
	PrintPixel("Programmatic CPU pixel1:", pixel1);

	float pixel2[4] = {0.3f, 0.1f, 0.5f, 0.9f};
	cpu->applyRGBA(pixel2);
	const float expect2[4] = {0.49f, 0.0625f, 0.4225f, 0.9f};
	if(!NearPixel(pixel2, expect2, 4))
		return false;
	PrintPixel("Programmatic CPU pixel2:", pixel2);

	float image[8] = {0.2f, 0.4f, 0.6f, 0.7f, 0.3f, 0.1f, 0.5f, 0.9f};
	OCIO::PackedImageDesc desc(image, 2, 1, 4);
	cpu->apply(desc);
	return NearPixel(image, expect1, 4) && NearPixel(image + 4, expect2, 4);
}

static bool TestInverseCPU(double& max_error)
{
	OCIO::ConstConfigRcPtr config = OCIO::Config::Create();
	OCIO::TransformRcPtr inverse = MakeProgrammaticGroup()->createEditableCopy();
	inverse->setDirection(OCIO::TRANSFORM_DIR_INVERSE);
	OCIO::ConstProcessorRcPtr fwd = config->getProcessor(MakeProgrammaticGroup());
	OCIO::ConstProcessorRcPtr inv = config->getProcessor(inverse);
	if(!fwd || !inv)
		return false;
	OCIO::ConstCPUProcessorRcPtr cpu_fwd = fwd->getDefaultCPUProcessor();
	OCIO::ConstCPUProcessorRcPtr cpu_inv = inv->getDefaultCPUProcessor();
	if(!cpu_fwd || !cpu_inv)
		return false;

	const float inputs[][4] = {
		{0.2f, 0.4f, 0.6f, 0.7f},
		{0.3f, 0.1f, 0.5f, 0.9f},
	};
	max_error = 0.0;
	for(const auto& in : inputs) {
		float pixel[4] = {in[0], in[1], in[2], in[3]};
		cpu_fwd->applyRGBA(pixel);
		cpu_inv->applyRGBA(pixel);
		for(int i = 0; i < 4; ++i)
			max_error = std::max(max_error, std::fabs((double)pixel[i] - in[i]));
	}
	printf("Inverse CPU max error: %.6g\n", max_error);
	return max_error <= 2e-5;
}

static OCIO::ConstConfigRcPtr MakeYamlConfig()
{
	const char* yaml =
		"ocio_profile_version: 2\n"
		"environment: {}\n"
		"search_path:\n"
		"  - .\n"
		"roles:\n"
		"  default: linear\n"
		"  scene_linear: linear\n"
		"colorspaces:\n"
		"  - !<ColorSpace>\n"
		"    name: linear\n"
		"    isdata: false\n"
		"  - !<ColorSpace>\n"
		"    name: log\n"
		"    isdata: false\n"
		"    to_scene_reference: !<GroupTransform>\n"
		"      children:\n"
		"        - !<MatrixTransform> {matrix: [2., 0., 0., 0., 0., 0.5, 0., 0., 0., 0., 1.5, 0., 0., 0., 0., 1.], offset: [0.1, 0.2, -0.1, 0.]}\n"
		"        - !<ExponentTransform> {value: [2., 2., 2., 1.]}\n"
		"        - !<RangeTransform> {min_in_value: 0., min_out_value: 0., max_in_value: 1., max_out_value: 1.}\n"
		"displays:\n"
		"  sRGB:\n"
		"    - !<View> {name: Raw, colorspace: linear}\n"
		"active_displays: [sRGB]\n"
		"active_views: [Raw]\n";
	std::istringstream in(yaml);
	return OCIO::Config::CreateFromStream(in);
}

static bool TestYamlConfig()
{
	OCIO::ConstConfigRcPtr config = MakeYamlConfig();
	if(!config)
		return false;
	config->validate();
	if(config->getNumColorSpaces() != 2)
		return false;
	const char* scene_linear = config->getRoleColorSpace("scene_linear");
	const char* default_role = config->getRoleColorSpace("default");
	if(!scene_linear || std::strcmp(scene_linear, "linear") != 0)
		return false;
	if(!default_role || std::strcmp(default_role, "linear") != 0)
		return false;
	if(!HasColorSpace(config, "linear"))
		return false;
	if(!HasColorSpace(config, "log"))
		return false;

	OCIO::ConstProcessorRcPtr processor = config->getProcessor("log", "linear");
	if(!processor)
		return false;
	OCIO::ConstCPUProcessorRcPtr cpu = processor->getDefaultCPUProcessor();
	if(!cpu)
		return false;

	float pixel[4] = {0.2f, 0.4f, 0.6f, 0.7f};
	cpu->applyRGBA(pixel);
	const float expect[4] = {0.25f, 0.16f, 0.64f, 0.7f};
	if(!NearPixel(pixel, expect, 4))
		return false;
	PrintPixel("YAML CPU pixel:", pixel);

	std::ostringstream out;
	config->serialize(out);
	std::istringstream in(out.str());
	OCIO::ConstConfigRcPtr reparsed = OCIO::Config::CreateFromStream(in);
	if(!reparsed)
		return false;
	reparsed->validate();
	printf("YAML parse/reparse: OK\n");
	return reparsed->getNumColorSpaces() == 2
		&& std::strcmp(reparsed->getRoleColorSpace("scene_linear"), "linear") == 0
		&& std::strcmp(reparsed->getRoleColorSpace("default"), "linear") == 0;
}

static bool TestInvalidYaml()
{
	const char* bad = "ocio_profile_version: 2\ncolorspaces: [\n";
	try {
		std::istringstream in(bad);
		(void)OCIO::Config::CreateFromStream(in);
		return false;
	} catch(const OCIO::Exception& e) {
		return e.what() && *e.what();
	} catch(const std::exception& e) {
		return e.what() && *e.what();
	}
}

static bool ExtractGpu(const OCIO::ConstProcessorRcPtr& processor, OCIO::GpuLanguage lang,
	const char* label, unsigned& textures, unsigned& tex3d, unsigned& uniforms, std::size_t& shader_len)
{
	OCIO::ConstGPUProcessorRcPtr gpu = processor->getDefaultGPUProcessor();
	OCIO::GpuShaderDescRcPtr desc = OCIO::GpuShaderDesc::CreateShaderDesc();
	if(!gpu || !desc)
		return false;
	desc->setLanguage(lang);
	desc->setFunctionName("OCIOTest");
	desc->setResourcePrefix("ocio_");
	gpu->extractGpuShaderInfo(desc);
	const char* shader = desc->getShaderText();
	if(!shader || !*shader)
		return false;
	shader_len = std::strlen(shader);
	textures = desc->getNumTextures();
	tex3d = desc->getNum3DTextures();
	uniforms = desc->getNumUniforms();
	const char* cache_id = desc->getCacheID();
	if(!cache_id || !*cache_id)
		return false;
	const char* proc_cache = gpu->getCacheID();
	if(!proc_cache || !*proc_cache)
		return false;
	if(std::strstr(shader, "OCIOTest") == NULL)
		return false;
	if(uniforms == 0)
		return false;
	OCIO::GpuShaderDesc::UniformData data;
	const char* name = desc->getUniform(0, data);
	if(!name || !*name || data.m_type != OCIO::UNIFORM_DOUBLE)
		return false;
	printf("%s shader length: %zu\n", label, shader_len);
	printf("%s textures=%u 3d_textures=%u uniforms=%u\n", label, textures, tex3d, uniforms);
	return true;
}

static bool TestGpuExtraction()
{
	OCIO::ConstConfigRcPtr config = OCIO::Config::Create();
	OCIO::ConstProcessorRcPtr processor = config->getProcessor(MakeDynamicExposureTransform());
	if(!processor)
		return false;

	unsigned textures = 0, tex3d = 0, uniforms = 0;
	std::size_t shader_len = 0;
	if(!ExtractGpu(processor, OCIO::GPU_LANGUAGE_GLSL_4_0, "GLSL", textures, tex3d, uniforms, shader_len))
		return false;
	if(!ExtractGpu(processor, OCIO::GPU_LANGUAGE_HLSL_SM_5_0, "HLSL", textures, tex3d, uniforms, shader_len))
		return false;
	return true;
}

static bool TestDynamicProperty()
{
	OCIO::ConstConfigRcPtr config = OCIO::Config::Create();
	OCIO::ExposureContrastTransformRcPtr ec = MakeDynamicExposureTransform();
	OCIO::ConstProcessorRcPtr processor = config->getProcessor(ec);
	if(!processor)
		return false;
	OCIO::ConstCPUProcessorRcPtr cpu = processor->getDefaultCPUProcessor();
	if(!cpu || !cpu->hasDynamicProperty(OCIO::DYNAMIC_PROPERTY_EXPOSURE))
		return false;
	OCIO::DynamicPropertyRcPtr prop = cpu->getDynamicProperty(OCIO::DYNAMIC_PROPERTY_EXPOSURE);
	if(!prop)
		return false;
	OCIO::DynamicPropertyDoubleRcPtr exposure = OCIO::DynamicPropertyValue::AsDouble(prop);
	if(!exposure)
		return false;

	float pixel[4] = {0.1f, 0.2f, 0.3f, 0.4f};
	const float identity[4] = {0.1f, 0.2f, 0.3f, 0.4f};
	cpu->applyRGBA(pixel);
	if(!NearPixel(pixel, identity, 4))
		return false;
	exposure->setValue(1.0);
	const float doubled[4] = {0.2f, 0.4f, 0.6f, 0.4f};
	cpu->applyRGBA(pixel);
	if(!NearPixel(pixel, doubled, 4))
		return false;
	PrintPixel("Dynamic property CPU pixel:", pixel);

	OCIO::ConstGPUProcessorRcPtr gpu = processor->getDefaultGPUProcessor();
	if(!gpu)
		return false;
	OCIO::GpuShaderDescRcPtr desc = OCIO::GpuShaderDesc::CreateShaderDesc();
	desc->setLanguage(OCIO::GPU_LANGUAGE_GLSL_4_0);
	desc->setFunctionName("OCIOTest");
	desc->setResourcePrefix("ocio_");
	gpu->extractGpuShaderInfo(desc);
	printf("Dynamic property GPU uniforms: %u\n", desc->getNumUniforms());
	return desc->getNumUniforms() > 0 && desc->getShaderText() && *desc->getShaderText();
}

static bool ValidateBuiltinConfig(const char* name)
{
	try
	{
		OCIO::ConstConfigRcPtr config = OCIO::Config::CreateFromBuiltinConfig(name);
		if(!config)
			return false;
		config->validate();
		if(config->getNumColorSpaces() == 0)
			return false;
		const char* display = config->getDefaultDisplay();
		if(!display || !*display)
			return false;
		const char* view = config->getDefaultView(display);
		if(!view || !*view)
			return false;
		printf("Builtin config %s: OK (%d color spaces, default display '%s', default view '%s')\n",
			name, (int)config->getNumColorSpaces(), display, view);
		return true;
	}
	catch(const std::exception& e)
	{
		printf("Builtin config %s: FAIL (%s)\n", name, e.what());
		return false;
	}
	catch(...)
	{
		printf("Builtin config %s: FAIL\n", name);
		return false;
	}
}

static bool TestBuiltinRegistry()
{
	const OCIO::BuiltinConfigRegistry& configs = OCIO::BuiltinConfigRegistry::Get();
	const size_t count = configs.getNumBuiltinConfigs();
	printf("Builtin registry count: %zu\n", count);
	if(count != 8)
		return false;

	const char* required[] = {
		"cg-config-v4.0.0_aces-v2.0_ocio-v2.5",
		"studio-config-v4.0.0_aces-v2.0_ocio-v2.5",
	};
	for(const char* name : required) {
		bool found = false;
		for(size_t i = 0; i < count; ++i) {
			const char* current = configs.getBuiltinConfigName(i);
			if(current && std::strcmp(current, name) == 0) {
				found = true;
				break;
			}
		}
		if(!found)
			return false;
	}

	if(!ValidateBuiltinConfig("cg-config-v4.0.0_aces-v2.0_ocio-v2.5"))
		return false;
	if(!ValidateBuiltinConfig("studio-config-v4.0.0_aces-v2.0_ocio-v2.5"))
		return false;

	return true;
}

int main()
{
	try {
	int passed = 0;
	int failed = 0;
	#ifdef UPP_IMAGING_LOCAL_OPENCOLORIO_SOURCE_INCLUDE
	printf("Source package marker: OK\n");
	passed++;
	#else
	printf("Source package marker: FAIL\n");
	failed++;
	#endif

	if(TestVersion()) {
		printf("OpenColorIO version: OK\n");
		passed++;
	} else {
		printf("OpenColorIO version: FAIL\n");
		failed++;
	}

	if(TestCreation()) {
		printf("Config creation: OK\n");
		printf("Transform creation: OK\n");
		printf("Processor creation: OK\n");
		printf("CPU processor creation: OK\n");
		printf("GPU processor creation: OK\n");
		printf("GPU shader descriptor creation: OK\n");
		passed += 6;
	} else {
		printf("Config/transform/processor creation: FAIL\n");
		failed += 6;
	}

	if(TestProgrammaticCPU()) {
		printf("OpenColorIO programmatic CPU transform: OK\n");
		printf("OpenColorIO packed image transform: OK\n");
		passed += 2;
	} else {
		printf("OpenColorIO programmatic CPU transform: FAIL\n");
		printf("OpenColorIO packed image transform: FAIL\n");
		failed += 2;
	}

	double max_error = 0.0;
	if(TestInverseCPU(max_error)) {
		printf("OpenColorIO inverse transform: OK (max error %.6g)\n", max_error);
		passed++;
	} else {
		printf("OpenColorIO inverse transform: FAIL (max error %.6g)\n", max_error);
		failed++;
	}

	if(TestYamlConfig()) {
		printf("OpenColorIO YAML config parse: OK\n");
		passed++;
	} else {
		printf("OpenColorIO YAML config parse: FAIL\n");
		failed++;
	}

	if(TestInvalidYaml()) {
		printf("OpenColorIO YAML config invalid rejection: OK\n");
		passed++;
	} else {
		printf("OpenColorIO YAML config invalid rejection: FAIL\n");
		failed++;
	}

	if(TestGpuExtraction()) {
		printf("OpenColorIO GPU GLSL extraction: OK\n");
		printf("OpenColorIO GPU HLSL extraction: OK\n");
		passed += 2;
	} else {
		printf("OpenColorIO GPU GLSL extraction: FAIL\n");
		printf("OpenColorIO GPU HLSL extraction: FAIL\n");
		failed += 2;
	}

	if(TestDynamicProperty()) {
		printf("OpenColorIO dynamic property CPU: OK\n");
		printf("OpenColorIO dynamic property GPU uniform: OK\n");
		passed += 2;
	} else {
		printf("OpenColorIO dynamic property CPU: FAIL\n");
		printf("OpenColorIO dynamic property GPU uniform: FAIL\n");
		failed += 2;
	}

	if(TestBuiltinRegistry()) {
		printf("OpenColorIO builtin registry: OK\n");
		passed++;
	} else {
		printf("OpenColorIO builtin registry: FAIL\n");
		failed++;
	}

	printf("SUMMARY passed=%d failed=%d\n", passed, failed);
	return failed ? 1 : 0;
	} catch(const OCIO::Exception& e) {
		printf("OpenColorIO exception: %s\n", e.what());
	} catch(const std::exception& e) {
		printf("std::exception: %s\n", e.what());
	}
	return 1;
}
