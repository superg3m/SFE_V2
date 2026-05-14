Officiall I consider this iteration of my engine done

here is what to do for the next iteration:
SFE_V3:
	- [] MUCH MORE STUFF IN USER SPACE
	- [] Drop hot code reloading (too much friction and compile times are like 1 second anyway with unity build just not worth it)
		- On mac run time type info is messed up across dll boundaries

	- [] Approaching Zero Driver Overhead in OpenGL (Check if VAO is already bound for example)
	- [] Bullet-proof allocator stuff (remember when when you used frame allocator in some cases with vector you had, what seemed like, random ass behavior)
	- [] Serialization (this should be like priority number one in next engine, serialize a scene)
	- [] Read up on animations and implement animation
	- [] Framebuffer stuff
	- [] Systems instead of having each component update itself (components will now just be pure data no update method)
		- [] This mean that system will take in an entity manager and query component set/list (make this like O(N) at worst) (even though it should be O(1))
	- [] Remove this dogshit seperation between editor.
	- [] Make Shader, Texture, ect... make these like a base class and then you can make an openlg specific derived class
	- [] I really like the mega files with like types and stuff I think its very apropos.
	- [] Unity build is really good, keep compile time low
	- [] Implement Audio
	- [] Maybe see if using singletons is worth it? (I can do this now because I don't have a dll)
	- [] Request system obviously can be thrown out
	- [] Multiple Cameras (camera's as entities) (Should be able to toggle the active camera and switch view points)
	- [] Dx11 backend
	- [] ParticleEmitter 
	- [] Shadow mapping

	- [] I can probably get away with allowing entitys to have a pointer to their parent because passing around the manager for that is really annoying just to get world space
	- [] Profile and code instrumentation in imgui
	- [] Job system (Load assets on another thread return a MeshHandle and then switch out the underlying slot)
	- [] In theory the Handles can actually hold the commonalities? Then the mesh slot can hold the api specific stuff
	- [] Scene system with nice lighting abilities (spotlight, sunlight, pointlights)
		- [] directional light
		- [] spotlight
		- [x] pointlight


	- [] Shader Header is a really interesting idea:
	```
		u8* shader_header_data = Platform::read_entire_file("shader_header.h")
		u8* shader_vert_source = Platform::read_entire_file("shader.vert")
		u8* shader_frag_source = Platform::read_entire_file("shader.frag")
		String::append(shader_header_data, length, shader_vert_source);
		String::append(shader_header_data, length, shader_frag_source);

		shader_header.h: {
			vec3 CalcDirLight(const DirLight light, const vec3 normal, const vec3 viewDir, const vec2 texCoord) {
				vec3 lightDir = normalize(-light.direction); // Directional light direction is inverse of uniform value

				// Diffuse calculation
				float diffuse_factor = max(dot(normal, lightDir), 0.0f);

				// Specular calculation
				vec3 reflectDir = reflect(-lightDir, normal); // Light direction, not negative light direction
				float specular_factor = pow(max(dot(viewDir, reflectDir), 0.0f), uMaterial.shininess);

				// Get material colors
				vec3 diffuse_mat_color = getDiffuseColor(texCoord);
				vec3 specular_mat_color = getSpecularColor(texCoord);

				// Combine light and material components
				vec3 ambient = light.ambient * (uMaterial.ambient_color + diffuse_mat_color); // Ambient usually uses diffuse color as well
				vec3 diffuse = light.diffuse * diffuse_factor * diffuse_mat_color;
				vec3 specular = light.specular * specular_factor * specular_mat_color;

				return (ambient + diffuse + specular);
			}
	```