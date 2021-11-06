struct wvp_const_buffer_data
{
    mat4 World;
    mat4 View;
    mat4 Proj; 
};

struct game_state
{
    window *Window;
    renderer *Renderer;
    arena EngineArena;
    arena ConstBufferArena;

    const_buffer *WvpConstBuffer; 
};
