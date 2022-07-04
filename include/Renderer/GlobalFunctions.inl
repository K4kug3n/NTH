

#if !defined(NTH_GLOBAL_FUNCTION)
#define NTH_GLOBAL_FUNCTION( fun )
#endif

#if !defined(NTH_GLOBAL_FUNCTION_OPTIONAL)
#define NTH_GLOBAL_FUNCTION_OPTIONAL( fun )
#endif

NTH_GLOBAL_FUNCTION(vkCreateInstance)
NTH_GLOBAL_FUNCTION(vkEnumerateInstanceExtensionProperties)
NTH_GLOBAL_FUNCTION(vkEnumerateInstanceLayerProperties)
NTH_GLOBAL_FUNCTION_OPTIONAL(vkEnumerateInstanceVersion)

#undef NTH_GLOBAL_FUNCTION
#undef NTH_GLOBAL_FUNCTION_OPTIONAL