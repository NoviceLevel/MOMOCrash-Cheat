#pragma once
#pragma warning(disable : 4311)
#pragma warning(disable : 4302)
#pragma warning(disable : 4312)
#include "pch.h"

#if FRAMEWORK_UNITY

#include "..\mono\metadata\threads.h"
#include "..\mono\metadata\object.h"

typedef MonoThread* (*t_mono_thread_attach)(MonoDomain* domain);
typedef MonoDomain* (*t_mono_get_root_domain)();
typedef MonoAssembly* (*t_mono_domain_assembly_open)(MonoDomain* doamin, const char* name);
typedef MonoImage* (*t_mono_assembly_get_image)(MonoAssembly* assembly);
typedef MonoClass* (*t_mono_class_from_name)(MonoImage* image, const char* name_space, const char* name);
typedef MonoMethod* (*t_mono_class_get_method_from_name)(MonoClass* klass, const char* name, int param_count);
typedef void* (*t_mono_compile_method)(MonoMethod* method);
typedef MonoObject* (*t_mono_runtime_invoke)(MonoMethod* method, void* obj, void** params, MonoObject** exc);
typedef MonoClassField* (*t_mono_class_get_field_from_name)(MonoClass* klass, const char* name);
typedef void* (*t_mono_field_get_value)(void* obj, MonoClassField* field, void* value);
typedef void (*t_mono_field_set_value)(MonoObject* obj, MonoClassField* field, void* value);
typedef MonoClass* (*t_mono_method_get_class)(MonoMethod* method);
typedef MonoVTable* (*t_mono_class_vtable)(MonoDomain* domain, MonoClass* klass);
typedef void* (*t_mono_vtable_get_static_field_data)(MonoVTable* vt);
typedef uint32_t (*t_mono_field_get_offset)(MonoClassField* field);
typedef MonoImage* (*t_mono_image_loaded)(const char* name);

class Mono
{
private:
	Mono() {};
	bool initalized = false;
	HMODULE hMono = nullptr;

	t_mono_thread_attach mono_thread_attach = nullptr;
	t_mono_get_root_domain mono_get_root_domain = nullptr;
	t_mono_domain_assembly_open mono_domain_assembly_open = nullptr;
	t_mono_assembly_get_image mono_assembly_get_image = nullptr;
	t_mono_class_from_name mono_class_from_name = nullptr;
	t_mono_class_get_method_from_name mono_class_get_method_from_name = nullptr;
	t_mono_compile_method mono_compile_method = nullptr;
	t_mono_runtime_invoke mono_runtime_invoke = nullptr;
	t_mono_class_get_field_from_name mono_class_get_field_from_name = nullptr;
	t_mono_field_get_value mono_field_get_value = nullptr;
	t_mono_field_set_value mono_field_set_value = nullptr;
	t_mono_method_get_class mono_method_get_class = nullptr;
	t_mono_class_vtable mono_class_vtable = nullptr;
	t_mono_vtable_get_static_field_data mono_vtable_get_static_field_data = nullptr;
	t_mono_field_get_offset mono_field_get_offset = nullptr;
	t_mono_image_loaded mono_image_loaded = nullptr;
	
	MonoImage* pCachedAssemblyCSharp = nullptr;

	void Initalize()
	{
		hMono = GetModuleHandleA("mono-2.0-bdwgc.dll");
		if (hMono == NULL) hMono = GetModuleHandleA("mono.dll");
		if (hMono == NULL) hMono = GetModuleHandleA("mono-2.0.dll");
		if (hMono == NULL) return;

		mono_domain_assembly_open = reinterpret_cast<t_mono_domain_assembly_open>(GetProcAddress(hMono, "mono_domain_assembly_open"));
		mono_assembly_get_image = reinterpret_cast<t_mono_assembly_get_image>(GetProcAddress(hMono, "mono_assembly_get_image"));
		mono_class_from_name = reinterpret_cast<t_mono_class_from_name>(GetProcAddress(hMono, "mono_class_from_name"));
		mono_class_get_method_from_name = reinterpret_cast<t_mono_class_get_method_from_name>(GetProcAddress(hMono, "mono_class_get_method_from_name"));
		mono_compile_method = reinterpret_cast<t_mono_compile_method>(GetProcAddress(hMono, "mono_compile_method"));
		mono_runtime_invoke = reinterpret_cast<t_mono_runtime_invoke>(GetProcAddress(hMono, "mono_runtime_invoke"));
		mono_class_get_field_from_name = reinterpret_cast<t_mono_class_get_field_from_name>(GetProcAddress(hMono, "mono_class_get_field_from_name"));
		mono_field_get_value = reinterpret_cast<t_mono_field_get_value>(GetProcAddress(hMono, "mono_field_get_value"));
		mono_field_set_value = reinterpret_cast<t_mono_field_set_value>(GetProcAddress(hMono, "mono_field_set_value"));
		mono_method_get_class = reinterpret_cast<t_mono_method_get_class>(GetProcAddress(hMono, "mono_method_get_class"));
		mono_class_vtable = reinterpret_cast<t_mono_class_vtable>(GetProcAddress(hMono, "mono_class_vtable"));
		mono_vtable_get_static_field_data = reinterpret_cast<t_mono_vtable_get_static_field_data>(GetProcAddress(hMono, "mono_vtable_get_static_field_data"));
		mono_field_get_offset = reinterpret_cast<t_mono_field_get_offset>(GetProcAddress(hMono, "mono_field_get_offset"));
		mono_image_loaded = reinterpret_cast<t_mono_image_loaded>(GetProcAddress(hMono, "mono_image_loaded"));
		mono_thread_attach = reinterpret_cast<t_mono_thread_attach>(GetProcAddress(hMono, "mono_thread_attach"));
		mono_get_root_domain = reinterpret_cast<t_mono_get_root_domain>(GetProcAddress(hMono, "mono_get_root_domain"));
		
		if (mono_thread_attach != nullptr && mono_get_root_domain != nullptr)
			mono_thread_attach(mono_get_root_domain());

		this->initalized = true;
	}

public:
	static Mono& Instance()
	{
		static Mono _instance;
		if (!_instance.initalized)
			_instance.Initalize();
		return _instance;
	}

	MonoImage* GetImage(const char* assemblyName)
	{
		if (strcmp(assemblyName, "Assembly-CSharp") == 0 && pCachedAssemblyCSharp != nullptr)
			return pCachedAssemblyCSharp;

		MonoDomain* pDomain = mono_get_root_domain();
		if (pDomain == nullptr) return nullptr;

		MonoImage* pImage = nullptr;
		MonoAssembly* pAssembly = nullptr;

		if (mono_image_loaded != nullptr)
		{
			pImage = mono_image_loaded(assemblyName);
			if (pImage != nullptr)
			{
				if (strcmp(assemblyName, "Assembly-CSharp") == 0)
					pCachedAssemblyCSharp = pImage;
				return pImage;
			}
		}

		std::string dllName = std::string(assemblyName) + ".dll";
		pAssembly = mono_domain_assembly_open(pDomain, dllName.c_str());
		if (pAssembly != nullptr)
		{
			pImage = mono_assembly_get_image(pAssembly);
			if (pImage != nullptr)
			{
				if (strcmp(assemblyName, "Assembly-CSharp") == 0)
					pCachedAssemblyCSharp = pImage;
				return pImage;
			}
		}

		std::string managedPath = "MomottoCrash_Data/Managed/" + dllName;
		pAssembly = mono_domain_assembly_open(pDomain, managedPath.c_str());
		if (pAssembly != nullptr)
		{
			pImage = mono_assembly_get_image(pAssembly);
			if (pImage != nullptr)
			{
				if (strcmp(assemblyName, "Assembly-CSharp") == 0)
					pCachedAssemblyCSharp = pImage;
				return pImage;
			}
		}

		pAssembly = mono_domain_assembly_open(pDomain, assemblyName);
		if (pAssembly != nullptr)
		{
			pImage = mono_assembly_get_image(pAssembly);
			if (pImage != nullptr)
			{
				if (strcmp(assemblyName, "Assembly-CSharp") == 0)
					pCachedAssemblyCSharp = pImage;
				return pImage;
			}
		}

		return nullptr;
	}

	void* GetCompiledMethod(const char* className, const char* methodName, int param_count = 0, const char* assemblyName = "Assembly-CSharp", const char* nameSpace = "MomottoCrash")
	{
		mono_thread_attach(mono_get_root_domain());

		MonoImage* pImage = GetImage(assemblyName);
		if (pImage == nullptr) return nullptr;

		MonoClass* pKlass = mono_class_from_name(pImage, nameSpace, className);
		if (pKlass == nullptr) return nullptr;

		MonoMethod* pMethod = mono_class_get_method_from_name(pKlass, methodName, param_count);
		if (pMethod == nullptr) return nullptr;

		return mono_compile_method(pMethod);
	}

	MonoMethod* GetMethod(const char* className, const char* methodName, int param_count = 0, const char* assemblyName = "Assembly-CSharp", const char* nameSpace = "MomottoCrash")
	{
		MonoImage* pImage = GetImage(assemblyName);
		if (pImage == nullptr) return nullptr;

		MonoClass* pKlass = mono_class_from_name(pImage, nameSpace, className);
		if (pKlass == nullptr) return nullptr;

		return mono_class_get_method_from_name(pKlass, methodName, param_count);
	}

	MonoClass* GetClass(const char* className, const char* assemblyName = "Assembly-CSharp", const char* nameSpace = "MomottoCrash")
	{
		MonoImage* pImage = GetImage(assemblyName);
		if (pImage == nullptr) return nullptr;
		return mono_class_from_name(pImage, nameSpace, className);
	}

	MonoClass* GetClassFromMethod(MonoMethod* method) { return mono_method_get_class(method); }

	MonoClassField* GetField(const char* className, const char* fieldName, const char* assemblyName = "Assembly-CSharp", const char* nameSpace = "MomottoCrash")
	{
		MonoImage* pImage = GetImage(assemblyName);
		if (pImage == nullptr) return nullptr;

		MonoClass* pKlass = mono_class_from_name(pImage, nameSpace, className);
		if (pKlass == nullptr) return nullptr;

		return mono_class_get_field_from_name(pKlass, fieldName);
	}
	
	MonoClassField* GetField(MonoClass* pKlass, const char* fieldName) { return mono_class_get_field_from_name(pKlass, fieldName); }
	uint32_t GetFieldOffset(MonoClassField* field) { return mono_field_get_offset(field); }
	void GetFieldValue(void* instance, MonoClassField* field, void* out) { mono_field_get_value(instance, field, out); }
	void SetFieldValue(MonoObject* obj, MonoClassField* field, void* value) { mono_field_set_value(obj, field, value); }
	MonoVTable* GetVTable(MonoClass* pKlass) { return mono_class_vtable(mono_get_root_domain(), pKlass); }
	void* GetStaticFieldData(MonoVTable* pVTable) { return mono_vtable_get_static_field_data(pVTable); }

	void* GetStaticFieldData(MonoClass* pKlass)
	{
		MonoVTable* pVTable = GetVTable(pKlass);
		if (pVTable == nullptr) return nullptr;
		return mono_vtable_get_static_field_data(pVTable);
	}

	void* GetStaticFieldValue(const char* className, const char* fieldName, const char* assemblyName = "Assembly-CSharp", const char* nameSpace = "MomottoCrash")
	{
		MonoClass* pKlass = GetClass(className, assemblyName, nameSpace);
		if (pKlass == nullptr) return nullptr;

		MonoClassField* pField = GetField(pKlass, fieldName);
		if (pField == nullptr) return nullptr;

		uintptr_t addr = reinterpret_cast<uintptr_t>(GetStaticFieldData(pKlass));
		uint32_t offset = GetFieldOffset(pField);
		return reinterpret_cast<void*>(addr + offset);
	}

	MonoObject* Invoke(MonoMethod* method, void* obj, void** params)
	{
		mono_thread_attach(mono_get_root_domain());
		MonoObject* exc;
		return mono_runtime_invoke(method, obj, params, &exc);
	}
};

#endif
