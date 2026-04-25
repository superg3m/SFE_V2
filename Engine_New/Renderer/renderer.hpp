template <typename Backend>
class Renderer {
public:
    TextureHandle CreateTexture(const TextureDesc& desc) {
        return Backend::create_texture(textures, desc);
    }

    void DestroyTexture(TextureHandle h) {
        Backend::destroy_texture(textures, h);
    }

    Backend::CommandBuffer begin_frame(u32 framebuffer = 0);
    Backend::CommandBuffer end_frame();
private:
    Registry<Backend::Texture> textures;
};