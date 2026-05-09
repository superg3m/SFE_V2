#version 330 core
out vec4 FragColor;

in vec2 v_TexCoord;
in vec3 v_WorldPosition;
in vec3 v_Normal;

struct Material {
	sampler2D albedo;
	bool has_albedo;

	sampler2D specular;
	bool has_specular;

	float metallic;
	float roughness;
	float ao;

	float opacity; // 1.0 fully opaque, 0.0 full transparent
	float shininess; // power of two
};

uniform Material uMaterial;


// TODO(Jovanni): Actually understand this

struct PointLight {
    vec3 position;

    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
};

#define MAX_LIGHT_COUNT 4

uniform int uPointLightCount;
uniform PointLight uPointLights[MAX_LIGHT_COUNT]; // TODO(Jovanni): Make a shader header that has a define for this that works for bot hthe c code and glsl
uniform vec3 uCameraPosition;
const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// float specular_mask = mpow(max(dot(V, R), 0.0), uMaterial.shininess);

// ----------------------------------------------------------------------------
void main() {
	vec3 N = normalize(v_Normal);
    vec3 V = normalize(uCameraPosition - v_WorldPosition);

	vec3 albedo_sample = uMaterial.has_albedo ? texture(uMaterial.albedo, v_TexCoord).rgb : vec3(1);
	vec3 specular_sample = uMaterial.has_specular ? texture(uMaterial.specular, v_TexCoord).rgb : vec3(1);

	vec4 result = vec4(albedo_sample, uMaterial.opacity);
	for(int i = 0; i < uPointLightCount; i++) {
		vec3 L = normalize(uPointLights[i].position - v_WorldPosition);
		vec3 R = reflect(-L, N);
		vec3 H = normalize(L + V);
		float lambertion_mask = max(dot(N, L), 0.0);
		float specular_mask = pow(max(dot(N, H), 0.0), uMaterial.shininess); // binn-phong

		vec3 ambient = uPointLights[i].ambient_color * albedo_sample;
   	 	vec3 diffuse = uPointLights[i].diffuse_color * lambertion_mask * albedo_sample;
		vec3 specular = uPointLights[i].specular_color * specular_mask * specular_sample;

		result += vec4(ambient + diffuse + specular, 0);
	}

	FragColor = result;
	return;

	/*
    vec3 N = normalize(v_Normal);
    vec3 V = normalize(uCameraPosition - v_WorldPosition);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
	vec3 albedo = pow(texture(uMaterial.albedo, v_TexCoord).rgb, 2.2);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < uLightCount; ++i) {
        // calculate per-light radiance
        vec3 L = normalize(uLight[i].position - v_WorldPosition);
        vec3 H = normalize(V + L);
        float distance = length(uLight[i].position - v_WorldPosition);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = uLight[i].color * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, uMaterial.opacity);
	*/
}