#include "Game.h"
#include <thread>
void Generate_Instance(Model& m, int amount);
void Draw_Instance(Model& m, int amount, Shader& shader);

template <typename T1, typename T2, typename T3>
void Game::Gaussian_Blured_Texture(int j, unsigned int times, Shader& blooming_blurshader, T1& PreShadowFBO, T2& shadow_blur_horizontalFBO,
    T3& shadow_blur_verticalFBO, Renderer& renderer, VertexArray& quadVa)
{
    bool horizontal = true, first_iteration = true;
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    blooming_blurshader.Bind();
    blooming_blurshader.SetUniform1f("halox", keyinput.bloom_halox);
    blooming_blurshader.SetUniform1f("haloy", keyinput.bloom_haloy);
    blooming_blurshader.SetUniform1f("haloz", keyinput.bloom_haloz);
    blooming_blurshader.SetUniform1i("radius", keyinput.bloom_radius);
    for (unsigned int i = 0; i < times; i++)
    {
        blooming_blurshader.SetUniform1i("horizontal", horizontal);
        if (first_iteration)
        {
            shadow_blur_horizontalFBO.Bind();
            first_iteration = false;
            PreShadowFBO.BindTexture(0, j);
        }
        else
            if (horizontal)
            {
                shadow_blur_horizontalFBO.Bind();
                shadow_blur_verticalFBO.BindTexture();
            }
            else
            {
                shadow_blur_verticalFBO.Bind();
                shadow_blur_horizontalFBO.BindTexture();
            }
        renderer.DrawArray(quadVa, blooming_blurshader);
        horizontal = !horizontal;
    }
    shadow_blur_verticalFBO.UnBind();
}

void Game::Update_Sun()
{
    float sun_height = 20;
    float east_x = static_cast<float>(sin(glfwGetTime() * keyinput.sun_speed)) * sun_height;
    float west_y = static_cast<float>(cos(glfwGetTime() * keyinput.sun_speed)) * sun_height;
    sun.Sun_Position = glm::vec3(east_x, west_y, -10.0f) + camera.Position;
    sun.Sun_Direction = -glm::vec3(east_x, west_y, -10.0f);
}

void Game::Update_Pointlight()
{
    pointlight.lightintensity = keyinput.pointlight_Intensity;
    pointlight.far_plane = keyinput.point_far_plane;
    for (int i = 0; i < POINT_LIGHTS_NUM; i++)
        pointlight.color[i] = keyinput.point_color;

}
void Game::Initialize_Pointlight()
{

    for (int i = 0; i < POINT_LIGHTS_NUM; i++)
    {
        pointlight.position[i] = glm::vec3(i * 3, 2, 0);
        pointlight.space[i].Translate(pointlight.position[i]);
        pointlight.space[i].Scale(glm::vec3(0.3f));
    }
    for (int i = 0; i < POINT_LIGHTS_NUM; i++)
    {
        pointlight.color[i] = glm::vec3(1.0f);
    }
    pointlight.lightintensity = keyinput.pointlight_Intensity;

}
void Game::Initialize_Models_Positions()
{

    models->Nano.position.Scale(glm::vec3(0.1f));
    models->Nano.position.Rotate(-90.0, glm::vec3(1.0, 0.0, 0.0));
    models->Nano.position.Translate(glm::vec3(0.0f, 1.0f, 0.0f));
    models->Marry.position.Scale(0.5f);
    models->Marry.position.Translate(glm::vec3(5.0f, 0.0f, 0.0f));

    models->Planet.position.Translate(glm::vec3(10.0f, 0, 0));
    models->Floor.position.Rotate(-90.0, glm::vec3(1.0, 0.0, 0.0));
    models->Sphere.position.Translate(glm::vec3(-2.0f, 1.0f, 0.0));

    std::unordered_map<std::string, Model*>::iterator iter;
    iter = models->models_map.begin();
    int i = 0;
    for (iter = models->anime_models_map.begin(); iter != models->anime_models_map.end(); iter++)
    {
        i++;
        iter->second->position.Translate(glm::vec3(0.0f, 0.0f, i * 5.0f));
        glm::mat4 rootTrans = models->animator_map[iter->first].RootTransform;
        iter->second->updateAABB(rootTrans);
    }
    for (iter = models->models_map.begin(); iter != models->models_map.end(); iter++)
    {
        iter->second->updateAABB();
    }

}

void Game::Update_Models_Positions()
{
    glm::mat4 mod;
    float theta = acos(glm::dot(glm::normalize(glm::vec3(camera.Front.x, 0.0, camera.Front.z)), glm::vec3(0.0f, 0.0f, -1.0f)));
    theta = camera.Front.x > 0 ? theta : static_cast<float>((2 * PI - theta));
    float character_view = static_cast<float>(theta * 180.0f / PI);
    ModelSpace modd;
    modd.Rotate(180.0f + character_view, glm::vec3(0.0, -1.0, 0.0));
    if (!camera.third_view)
    {
        mod = modd.GetModelSpace();
    }
    else
    {
        float rotation = 0;
        switch (camera.character_Front)
        {
        case FORWARD:
            rotation = 0;
            break;
        case LEFT:
            rotation = -90.0f;
            break;
        case RIGHT:
            rotation = 90.0f;
            break;
        case BACKWARD:
            rotation = 180.0f;
            break;
        case FORWARD_LEFT:
            rotation = -45.0f;
            break;
        case FORWARD_RIGHT:
            rotation = 45.0f;
            break;
        case BACKWARD_LEFT:
            rotation = 225.0f;
            break;
        case BACKWARD_RIGHT:
            rotation = 135.0f;
            break;
        }
        if (camera.is_move)
        {
            modd.Rotate(rotation, glm::vec3(0.0, -1.0, 0.0));
            mod = modd.GetModelSpace();
        }
        else
            mod = models->Main_character.position.GetModelSpace();
    }
    mod[3][0] = camera.Position.x;
    mod[3][1] = camera.Position.y - 1.6f;
    mod[3][2] = camera.Position.z;
    models->Main_character.position.SetModel(mod);
    models->Main_character.updateAABB();
    models->Main_character.isMoved = true;
}

void Game::Initialize_Vertex_Arrays()
{

    std::string floor_vertex_attr = "floor_vertex_attr";
    std::vector<float> floor_vertex = config.ReadVector(floor_vertex_attr);
    VertexBuffer floorVb(&floor_vertex[0], static_cast<unsigned int>(floor_vertex.size()) * sizeof(float));
    VertexBufferLayout floorLayout;
    floorLayout.Push<float>(3);
    floorLayout.Push<float>(3);
    floorLayout.Push<float>(2);
    floorLayout.Push<float>(3);
    floorLayout.Push<float>(3);
    models->Floor.va->AddBuffer(floorVb, floorLayout);
    //sphere
    sphere_data sphere = SphereData();
    models->Sphere.Get_index(&sphere.index[0], static_cast<unsigned int>(sphere.index.size()));
    VertexBuffer sphereVb(&sphere.vertex[0], sizeof(float) * static_cast<unsigned int>(sphere.vertex.size()));
    VertexBufferLayout sphereLayout;
    sphereLayout.Push<float>(3);//position
    sphereLayout.Push<float>(3);//normal
    sphereLayout.Push<float>(2);//texcoord
    models->Sphere.va->AddBuffer(sphereVb, sphereLayout);
    //quad
    std::string quad_vertex_attr = "quad_vertex_attr";
    std::vector<float> quad_vertex = config.ReadVector(quad_vertex_attr);
    VertexBuffer quadVb(&quad_vertex[0], static_cast<unsigned int>(quad_vertex.size()) * sizeof(float));
    VertexBufferLayout quadLayout;
    quadLayout.Push<float>(3);
    quadLayout.Push<float>(2);
    vertex_arrays->quadVa.AddBuffer(quadVb, quadLayout);
    //��պ�
    std::string skybox_vertex_attr = "skybox_vertex_attr";
    std::vector<float> skybox_vertex = config.ReadVector(skybox_vertex_attr);
    VertexBuffer skyboxVb(&skybox_vertex[0], static_cast<unsigned int>(skybox_vertex.size()) * sizeof(float));
    VertexBufferLayout skyboxLayout;
    skyboxLayout.Push<float>(3);
    vertex_arrays->skyboxVa.AddBuffer(skyboxVb, skyboxLayout);
    //cube
    std::string cube_vertex_attr = "cube_vertex_attr";
    std::vector<float> cube_vertex = config.ReadVector(cube_vertex_attr);
    VertexBuffer cubeVb(&cube_vertex[0], static_cast<unsigned int>(cube_vertex.size()) * sizeof(float));
    VertexBufferLayout cubeLayout;
    cubeLayout.Push<float>(3);
    cubeLayout.Push<float>(3);
    cubeLayout.Push<float>(2);
    vertex_arrays->cubeVa.AddBuffer(cubeVb, cubeLayout);
    //cubeQuad
    std::string cube_quad_vertex_attr = "cube_quad_vertex_attr";
    std::vector<float> cube_quad_vertex = config.ReadVector(cube_quad_vertex_attr);
    VertexBuffer cube_quadVb(&cube_quad_vertex[0], static_cast<unsigned int>(cube_quad_vertex.size()) * sizeof(float));
    VertexBufferLayout cube_quadLayout;
    cube_quadLayout.Push<float>(3);
    cube_quadLayout.Push<float>(3);
    cube_quadLayout.Push<float>(2);
    vertex_arrays->cubeQuadVa.AddBuffer(cube_quadVb, cube_quadLayout);
    //textVa
    VertexBufferLayout textLayout;
    textLayout.Push<float>(2);
    textLayout.Push<float>(2);
    vertex_arrays->textVa.AddBuffer(vertex_buffers->textVb, textLayout);
    //particleVa
    std::string particle_vertex_attr = "particle_vertex_attr";
    std::vector<float> particle_vertex = config.ReadVector(particle_vertex_attr);
    VertexBuffer particleVb(&particle_vertex[0], static_cast<unsigned int>(particle_vertex.size()) * sizeof(float));
    VertexBufferLayout particleLayout;
    particleLayout.Push<float>(2);
    particleLayout.Push<float>(2);
    vertex_arrays->particleVa.AddBuffer(particleVb, particleLayout);

}
void RootAnimation(glm::mat4& m1, glm::mat4& m2)
{
    m1[3][2] -= m2[3][2];
}
void Game::Generate_Dir_CSM()
{
    //int update_CSM_SPLIT_NUM = CSM_SPLIT_NUM - 1;//ÿ��֡����һ��3��csm
    //if (Last_CSM_update)
    //{
    //    update_CSM_SPLIT_NUM = CSM_SPLIT_NUM;
    //    Last_CSM_update_matrix = csm_dirlight.light_projection_matrix[3];
    //}
    //Last_CSM_update = !Last_CSM_update;
    Shader& shader = shaders->DirLightShadowshader;
    for (int i = 0; i < CSM_SPLIT_NUM; i++)
    {
        shader.Bind();
        shader.SetUniformmatri4fv("LightSpace", csm_dirlight.light_projection_matrix[i]);

        framebuffers->csm_mapFBO[i].Bind();//֡����洢��ͼ
        framebuffers->csm_mapFBO[i].SetViewPort();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //����       
        drawModelsShadow(shader);
        shader.SetUniformmatri4fv("model", models->Floor.position.GetModelSpace());
        renderer.DrawArray(*models->Floor.va, shader);

        shader.SetUniformmatri4fv("model", models->Sphere.position.GetModelSpace());
        renderer.DrawElement(*models->Sphere.va, *models->Sphere.ib, shader);
        framebuffers->csm_mapFBO[i].UnBind();

    }
    glViewport(0, 0, screenWidth, screenHeight);

}
void Game::Generate_Point_SM()
{
    Shader& shader = shaders->PointLightShadowshader;
    shader.Bind();
    glm::mat4 PointlightProjection = glm::perspective(glm::radians(90.0f), 1.0f, pointlight.near_plane, pointlight.far_plane);//aspectӦΪ��Ӱ���߱�
    for (int i = 0; i < POINT_LIGHTS_NUM; i++)
    {
        std::vector<glm::mat4> shadowTransforms;
        glm::vec3 position = pointlight.position[i];
        shadowTransforms.push_back(PointlightProjection *
            glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));//��
        shadowTransforms.push_back(PointlightProjection *
            glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));//��
        shadowTransforms.push_back(PointlightProjection *
            glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));//��
        shadowTransforms.push_back(PointlightProjection *
            glm::lookAt(position, position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));//��
        shadowTransforms.push_back(PointlightProjection *
            glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));//ǰ
        shadowTransforms.push_back(PointlightProjection *
            glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));//��

        for (int face = 0; face < 6; face++)
        {
            shader.SetUniformmatri4fv("shadowMatrices[" + std::to_string(face) + "]", shadowTransforms[face]);
        }
        shader.SetUniform3f("lightPos", position);
        shader.SetUniform1f("far_plane", pointlight.far_plane);
        framebuffers->PointlightMapfbo[i].Bind();//֡����洢��ͼ
        framebuffers->PointlightMapfbo[i].SetViewPort();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //����
        drawModelsShadow(shader);
        shader.SetUniformmatri4fv("model", models->Floor.position.GetModelSpace());
        renderer.DrawArray(*models->Floor.va, shader);

        shader.SetUniformmatri4fv("model", models->Sphere.position.GetModelSpace());
        renderer.DrawElement(*models->Sphere.va, *models->Sphere.ib, shader);
        framebuffers->PointlightMapfbo[i].UnBind();
    }
    glViewport(0, 0, screenWidth, screenHeight);

}
void Game::Generate_Spot_SM()
{
    Shader& shader = shaders->SpotLightShadowshader;
    shader.Bind();
    shader.SetUniformmatri4fv("LightSpace", spotlight.SpotlightSpaceMatrix);
    framebuffers->SpotlightMapfbo.Bind();//֡����洢��ͼ
    framebuffers->SpotlightMapfbo.SetViewPort();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //����
    drawModelsShadow(shader);

    shader.SetUniformmatri4fv("model", models->Floor.position.GetModelSpace());
    renderer.DrawArray(*models->Floor.va, shader);

    shader.SetUniformmatri4fv("model", models->Sphere.position.GetModelSpace());
    renderer.DrawElement(*models->Sphere.va, *models->Sphere.ib, shader);
    framebuffers->SpotlightMapfbo.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}

void Game::Generate_Defered_basicDATA()
{
    if (keyinput.show_mesh)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    Shader& shader = shaders->DeferedShader;
    shader.Bind();
    shader.SetUniformmatri4fv("view", camera.GetViewMatrix());
    shader.SetUniformmatri4fv("projection", camera.GetProjectionMatrix());
    if (camera.third_view)
        shader.SetUniform3f("viewPos", camera.Get_third_position());
    else
        shader.SetUniform3f("viewPos", camera.Get_first_position());
    shader.SetUniform2f("jitter", HaltonSequence[frameIndex]);
    shader.SetUniform2f("resolution", 1.0f / screenWidth, 1.0f / screenHeight);
    framebuffers->gbuffer.Bind();
    framebuffers->gbuffer.SetViewPort();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.SetUniform1i("use_NormalMap", keyinput.NormalMap);
    shader.SetUniform1i("use_HeightMap", keyinput.useheight);
    shader.SetUniform1f("metallic", keyinput.metallic);
    shader.SetUniform1f("roughness", keyinput.roughness);
    shader.SetUniform1i("is_instance", 0);
    drawModels(shader);
    shader.SetUniform1i("is_instance", 1);
    Draw_Instance(*models->models_map["Rock"], 1000, shader);
    shader.SetUniform1i("is_instance", 0);
    //�ذ�  
    shader.SetUniform1f("metallic", 0.7);
    shader.SetUniform1f("roughness", 0);
    textures->floor_diffuse.Bind(0);
    shader.SetUniform1i("material.texture_diffuse", 0);
    textures->floor_specular.Bind(1);
    shader.SetUniform1i("material.texture_specular", 1);
    textures->floor_normal.Bind(2);
    shader.SetUniform1i("material.texture_normal", 2);
    textures->floor_height.Bind(3);
    shader.SetUniform1i("material.texture_height", 3);
    shader.SetUniformmatri4fv("model", models->Floor.position.GetModelSpace());
    renderer.DrawArray(*models->Floor.va, shader);
    //����
    shader.SetUniformmatri4fv("model", models->Sphere.position.GetModelSpace());
    renderer.DrawElement(*models->Sphere.va, *models->Sphere.ib, shader);
    //cube
    /*glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    auto f = [&](Entity::Octree& o) {
        ModelSpace md;
        md.Scale(o.pointMax);
        shader.SetUniformmatri4fv("model", md.GetModelSpace());
        renderer.DrawQuads(vertex_arrays->cubeQuadVa, shader); 
    };
    std::function<void(Entity::Octree& octre)> f2 = [&f2, &f](Entity::Octree& octre) {
        for (Entity::Octree o : octre.children)
        {
            f(o);
            if (!o.children.empty())
                f2(o);
        }
    };
    f2(octree);*/
    if (keyinput.use_terrain)
    {
        Generate_Terrain_gpu();
    }
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    framebuffers->gbuffer.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}
void Game::Generate_SSAO()
{
    framebuffers->ssaoFBO.Bind();
    framebuffers->ssaoFBO.SetViewPort();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Shader& shader = shaders->SSAOShader;
    shader.Bind();
    shader.SetUniformmatri4fv("projection", camera.GetProjectionMatrix());
    shader.SetUniformmatri4fv("view", camera.GetViewMatrix());
    shader.SetUniform1f("camera.far_plane", camera.far_plane);
    shader.SetUniform1f("bias", keyinput.SSAO_bias);
    shader.SetUniform1f("radius", keyinput.SSAO_radius);
    shader.SetUniform1f("rangecheck", keyinput.SSAO_rangecheck);
    shader.SetUniform2f("resolution", screenWidth, screenHeight);

    framebuffers->cameradepthFBO.BindTexture(0);
    shader.SetUniform1i("camera.Depth", 0);
    framebuffers->gbuffer.BindTexture(1, 0);
    shader.SetUniform1i("gPosition", 1);
    framebuffers->gbuffer.BindTexture(2, 1);
    shader.SetUniform1i("gNormal", 2);
    framebuffers->ssaoFBO.BindNoiseTexture(3);
    shader.SetUniform1i("Noise", 3);
    for (int i = 0; i < 64; i++)
    {
        shader.SetUniform3f("samples[" + std::to_string(i) + "]", framebuffers->ssaoFBO.ssao_data.ssaokernel[i]);
    }
    renderer.DrawArray(vertex_arrays->quadVa, shader);
    framebuffers->ssaoFBO.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}
void Game::Generate_SSAO_blur()
{
    framebuffers->ssaoblurFBO.Bind();
    framebuffers->ssaoblurFBO.SetViewPort();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaders->SSAOBlurShader.Bind();
    framebuffers->ssaoFBO.BindTexture(0);
    shaders->SSAOBlurShader.SetUniform1i("ssaoInput", 0);
    renderer.DrawArray(vertex_arrays->quadVa, shaders->SSAOBlurShader);
    framebuffers->ssaoblurFBO.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}
void Game::Generate_PreShadow()
{
    Shader& shader = shaders->DeferedPreShadowShader;
    shader.Bind();
    shader.SetUniform3f("camera.viewPos", camera.Position);
    shader.SetUniform1f("camera.far_plane", camera.far_plane);
    shader.SetUniform1f("camera.near_plane", camera.near_plane);
    shader.SetUniform1f("sun_sm_bias", keyinput.sun_sm_bias);
    shader.SetUniform1f("point_sm_radius", keyinput.point_sm_radius);
    shader.SetUniform1f("point_sm_pcf", keyinput.point_sm_pcf);
    shader.SetUniform1i("sun_pcf", keyinput.sun_pcf);
    shader.SetUniform1f("sun_pcf_radius", keyinput.sun_pcf_radius);
    shader.SetUniform3f("dirlight.direction", sun.Sun_Direction);
    for (int i = 0; i < 4; i++)
    {
        shader.SetUniform3f("pointlight[" + std::to_string(i) + "].position", pointlight.position[i]);
        shader.SetUniform1f("pointlight[" + std::to_string(i) + "].far_plane", pointlight.far_plane);
    }
    framebuffers->cameradepthFBO.BindTexture(0);
    shader.SetUniform1i("camera.Depth", 0);
    framebuffers->gbuffer.BindTexture(1, 0);
    shader.SetUniform1i("gPosition", 1);
    framebuffers->gbuffer.BindTexture(2, 1);
    shader.SetUniform1i("gNormal", 2);
    framebuffers->gbuffer.BindTexture(3, 2);

    for (int i = 0; i < POINT_LIGHTS_NUM; i++)
    {
        framebuffers->PointlightMapfbo[i].BindTexture(i + 4);
        shader.SetUniform1i("shadowMap.PointShadow[" + std::to_string(i) + "]", i + 4);
    }
    for (int i = 0; i < CSM_SPLIT_NUM; i++)
    {
        shader.SetUniform1f("split_distance[" + std::to_string(i) + "]", csm_dirlight.Get_frustum_far(i));
        //shader.SetUniform1f("z_distance[" + std::to_string(i) + "]", csm_dirlight.Get_z_distance(i));
        shader.SetUniform1f("xy_distance[" + std::to_string(i) + "]", csm_dirlight.Get_xy_distance(i));
        framebuffers->csm_mapFBO[i].BindTexture(i + 8);
        shader.SetUniform1i("shadowMap.csm_map[" + std::to_string(i) + "]", i + 8);
        /*if (i == CSM_SPLIT_NUM - 1)
            shader.SetUniformmatri4fv("DirlightCSMMatrix[" + std::to_string(i) + "]", Last_CSM_update_matrix);
        else*/
            shader.SetUniformmatri4fv("DirlightCSMMatrix[" + std::to_string(i) + "]", csm_dirlight.light_projection_matrix[i]);

    }

    framebuffers->SpotlightMapfbo.BindTexture(12);
    shader.SetUniform1i("shadowMap.SpotShadow", 12);
    shader.SetUniformmatri4fv("SpotlightSpaceMatrix", spotlight.SpotlightSpaceMatrix);

    framebuffers->PreShadowFBO.Bind();//
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    renderer.DrawArray(vertex_arrays->quadVa, shader);
    glEnable(GL_DEPTH_TEST);
    framebuffers->PreShadowFBO.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}
void Game::Generate_Origin_Screen()
{
    Shader& shader = shaders->DeferedLighting_shader;
    shader.Bind();
    if (camera.third_view)
        shader.SetUniform3f("camera.viewPos", camera.Get_third_position());
    else
        shader.SetUniform3f("camera.viewPos", camera.Get_first_position());
    framebuffers->gbuffer.BindTexture(1, 0);
    shader.SetUniform1i("gPosition", 1);
    framebuffers->gbuffer.BindTexture(2, 1);
    shader.SetUniform1i("gNormal", 2);
    framebuffers->gbuffer.BindTexture(3, 2);
    shader.SetUniform1i("gAlbedoSpec", 3);
    pre_framebuffers->envcubemap_convolutionFBO.BindTexture(4);
    shader.SetUniform1i("EnvLight", 4);
    pre_framebuffers->envcubemap_spec_convolutionFBO.BindTexture(5);
    shader.SetUniform1i("EnvLight_spec", 5);
    pre_framebuffers->envcubemap_spec_BRDF_FBO.BindTexture(6);
    shader.SetUniform1i("EnvLight_spec_BRDF", 6);
    shader.SetUniform1i("use_EnvLight_spec", keyinput.EnvLight_spec);

    framebuffers->PreShadowFBO.BindTexture(8, 0);
    shader.SetUniform1i("predirshadow", 8);//ƽ�й�

    framebuffers->PreShadowFBO.BindTexture(9, 1);
    shader.SetUniform1i("prepointshadow", 9);//���
    framebuffers->PreShadowFBO.BindTexture(10, 2);
    shader.SetUniform1i("prespotshadow", 10);//�۹�
    shader.SetUniform1i("useSSAO", keyinput.useSSAO);
    if (keyinput.useSSAO)
    {
        framebuffers->ssaoblurFBO.BindTexture(13);
        shader.SetUniform1i("ssao", 13);
    }

    //ƽ�й�
    shader.SetUniform3f("dirlight.color", keyinput.SunColor * keyinput.SunIntensity);
    shader.SetUniform1f("dirlight.LightIntensity", keyinput.SunIntensity);
    shader.SetUniform3f("dirlight.direction", sun.Sun_Direction);
    //���
    for (int i = 0; i < 4; i++)
    {
        shader.SetUniform3f("pointlight[" + std::to_string(i) + "].color", pointlight.color[i] * pointlight.lightintensity);
        shader.SetUniform3f("pointlight[" + std::to_string(i) + "].position", pointlight.position[i]);
        shader.SetUniform1f("pointlight[" + std::to_string(i) + "].LightIntensity", pointlight.lightintensity);
        shader.SetUniform1f("pointlight[" + std::to_string(i) + "].far_plane", pointlight.far_plane);
        shader.SetUniform1f("pointlight[" + std::to_string(i) + "].near_plane", pointlight.near_plane);
    }
    //�۹��ֵ�
    shader.SetUniform3f("spotlight.color", keyinput.torch_color * (keyinput.torch_intensity * keyinput.TorchOn));
    shader.SetUniform3f("spotlight.direction", spotlight.direction);
    shader.SetUniform3f("spotlight.position", spotlight.position);
    shader.SetUniform1f("spotlight.far_plane", spotlight.far_plane);
    shader.SetUniform1f("spotlight.LightIntensity", keyinput.torch_intensity * keyinput.TorchOn);
    shader.SetUniform1f("spotlight.inner_CutOff", glm::cos(glm::radians(10.5f)));//spotlight��Χ
    shader.SetUniform1f("spotlight.outer_CutOff", glm::cos(glm::radians(12.5f)));//spotlight��Χ

    framebuffers->hdrfbo.Bind();//ʹ�÷�Ĭ��֡�������ṩHDR
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    renderer.DrawArray(vertex_arrays->quadVa, shader);
    glEnable(GL_DEPTH_TEST);
    framebuffers->hdrfbo.UnBind();
}
void Game::Generate_SkyBox()
{
    //��ȡ�����Ϣ
    if (keyinput.show_mesh)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    framebuffers->hdrfbo.Bind();
    framebuffers->gbuffer.Read();
    framebuffers->hdrfbo.Write();
    framebuffers->gbuffer.BlitDepthBuffer();
    Shader& shader = shaders->basic_shader;
    shader.Bind();
    shader.SetUniformmatri4fv("view", camera.GetViewMatrix());
    shader.SetUniformmatri4fv("projection", camera.GetProjectionMatrix());
    for (unsigned int i = 0; i < POINT_LIGHTS_NUM; i++)
    {
        shader.SetUniform3f("color", pointlight.color[i] * pointlight.lightintensity);
        shader.SetUniformmatri4fv("model", pointlight.space[i].GetModelSpace());
        renderer.DrawElement(*models->Sphere.va, *models->Sphere.ib, shader);
    }
    shader.SetUniform3f("color", keyinput.SunColor * keyinput.SunIntensity * glm::vec3(10));
    ModelSpace dirlightspace;//̫��λ�û�仯�����Զ�̬�ı�λ��       
    dirlightspace.Translate(sun.Sun_Position);
    //dirlightspace.Scale(glm::vec3(0.3f));
    shader.SetUniformmatri4fv("model", dirlightspace.GetModelSpace());
    renderer.DrawElement(*models->Sphere.va, *models->Sphere.ib, shader);
    //��պ�       
    shaders->skyboxShader.Bind();
    glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));//��ά�Ƴ������е�λ��
    shaders->skyboxShader.SetUniformmatri4fv("view", view);
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), float(screenWidth / screenHeight), camera.near_plane, camera.far_plane);
    shaders->skyboxShader.SetUniformmatri4fv("projection", projection);
    textures->skybox.Bind();
    //envcubemapFBO.BindTexture();
    //envcubemap_spec_convolutionFBO.BindTexture();
    renderer.DrawArray(vertex_arrays->skyboxVa, shaders->skyboxShader);
    if (keyinput.show_d3particle)
    {
        Generate_D3Particle();
        d3particle_generator.update_Particle();
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    framebuffers->hdrfbo.UnBind();
}
void Game::Generate_PostProcess()
{

    //
    framebuffers->blooming_hightlightFBO.Bind();
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //��ԭ���ֳ����ݣ���ԭ��(ambient + color * shadow * blocker_distance),��һ���Ǹ���highlight
    shaders->blooming_highlightshader.Bind();
    framebuffers->hdrfbo.BindTexture();
    shaders->blooming_highlightshader.SetUniform1i("screenTexture", 0);
    shaders->blooming_highlightshader.SetUniform1f("edge", keyinput.bloom_edge);
    renderer.DrawArray(vertex_arrays->quadVa, shaders->blooming_highlightshader);
    framebuffers->blooming_hightlightFBO.UnBind();
    //��������ͼ���и�˹ģ��
    Gaussian_Blured_Texture(1, keyinput.bloom_times, shaders->blooming_blurshader, framebuffers->blooming_hightlightFBO, framebuffers->blooming_blur_horizontalFBO, framebuffers->blooming_blur_verticalFBO, renderer, vertex_arrays->quadVa);
    //
    
    //���ԭ����ģ�����ͼƬ�γɷ���
    shaders->screenShader.Bind();
    shaders->screenShader.SetUniform1i("gamma", keyinput.gamma);
    shaders->screenShader.SetUniform1f("exposure", keyinput.exposure);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    framebuffers->blooming_hightlightFBO.BindTexture(0);
    shaders->screenShader.SetUniform1i("screenTexture", 0);
    framebuffers->blooming_blur_verticalFBO.BindTexture(1);
    shaders->screenShader.SetUniform1i("blooming_screenTexture", 1);
    framebuffers->cameradepthFBO.BindTexture(2);
    shaders->screenShader.SetUniform1i("assistTexture", 2);
    renderer.DrawArray(vertex_arrays->quadVa, shaders->screenShader);
    if (keyinput.assist_screen)
    {
        glViewport(0, 0, int(screenWidth * 0.4), int(screenHeight * 0.4));
        shaders->basicscreen_shader.Bind();
        framebuffers->MLAA_FBO.BindTexture(0);
        renderer.DrawArray(vertex_arrays->quadVa, shaders->basicscreen_shader);
        glViewport(0, 0, screenWidth, screenHeight);
    }
    //fxaa
    if (keyinput.fxaa_on)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        framebuffers->FXAA_FBO.Write();
        glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaders->FXAA_shader.Bind();
        shaders->FXAA_shader.SetUniform1i("fxaaOn", keyinput.fxaa_on);
        shaders->FXAA_shader.SetUniform2f("resolution", 1.0f / screenWidth, 1.0f / screenHeight);

        shaders->FXAA_shader.SetUniform1f("lumaThreshold", keyinput.fxaa_lumaThreshold);
        shaders->FXAA_shader.SetUniform1f("maxReduce", keyinput.fxaa_maxReduce);
        shaders->FXAA_shader.SetUniform1f("minReduce", keyinput.fxaa_minReduce);
        shaders->FXAA_shader.SetUniform1f("maxSpan", keyinput.fxaa_maxSpan);
        framebuffers->FXAA_FBO.BindTexture();
        renderer.DrawArray(vertex_arrays->quadVa, shaders->FXAA_shader);
    }
    //TAA
    if (keyinput.taa_on)
    {
        ////currentcolor
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        framebuffers->TAA_currentFrame_FBO.Write();
        glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        framebuffers->TAA_currentFrame_FBO.UnBind();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Shader& shader = shaders->TAA_shader;
        shader.Bind();

        shader.SetUniform1f("mixWeight", keyinput.taa_mixWeight);
        framebuffers->TAA_preFrame_FBO.BindTexture(0);
        shader.SetUniform1i("preFrameColor", 0);

        framebuffers->TAA_currentFrame_FBO.BindTexture(1);
        shader.SetUniform1i("currentFrameColor", 1);

        renderer.DrawArray(vertex_arrays->quadVa, shader);
        //precolor
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        framebuffers->TAA_preFrame_FBO.Write();
        glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        framebuffers->TAA_preFrame_FBO.UnBind();

        frameIndex++;
        if (frameIndex >= Halton_Num)
            frameIndex = 0;
    }
    if (keyinput.mlaa_on)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        framebuffers->exchange_FBO.Write();
        glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        framebuffers->exchange_FBO.UnBind();
        //generate signal
        framebuffers->MLAA_FBO.Bind();
        shaders->MLAA_first_shader.Bind();
        shaders->MLAA_first_shader.SetUniform1f("threShold", keyinput.mlaa_threShold);
        framebuffers->exchange_FBO.BindTexture();
        renderer.DrawArray(vertex_arrays->quadVa, shaders->MLAA_first_shader);
        framebuffers->MLAA_FBO.UnBind();

        shaders->MLAA_second_shader.Bind();
        shaders->MLAA_second_shader.SetUniform2f("resolution", 1.0f / screenWidth, 1.0f / screenHeight);
        shaders->MLAA_second_shader.SetUniform1i("searchNum", keyinput.mlaa_searchNum);
        framebuffers->MLAA_FBO.BindTexture(0);
        shaders->MLAA_second_shader.SetUniform1i("firstTexture", 0);
        framebuffers->exchange_FBO.BindTexture(1);
        shaders->MLAA_second_shader.SetUniform1i("colorTexture", 1);
        renderer.DrawArray(vertex_arrays->quadVa, shaders->MLAA_second_shader);
    }
}

void Game::Generate_CubeTexture()
{
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
    shaders->EnvCubeMapShader.Bind();
    shaders->EnvCubeMapShader.SetUniformmatri4fv("projection", captureProjection);
    pre_framebuffers->envcubemapFBO.SetViewPort();
    textures->equirectangularMap.Bind();
    for (unsigned int i = 0; i < 6; ++i)
    {
        pre_framebuffers->envcubemapFBO.Bind(i);
        shaders->EnvCubeMapShader.SetUniformmatri4fv("view", captureViews[i]);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderer.DrawArray(vertex_arrays->cubeVa, shaders->EnvCubeMapShader);
    }
    pre_framebuffers->envcubemapFBO.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);

}

void Game::Generate_EnvLight_Diffuse()
{
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
    shaders->EnvCubeMap_ConvolutionShader.Bind();
    shaders->EnvCubeMap_ConvolutionShader.SetUniformmatri4fv("projection", captureProjection);
    pre_framebuffers->envcubemap_convolutionFBO.SetViewPort();
    textures->skybox.Bind();
    for (unsigned int i = 0; i < 6; ++i)
    {
        pre_framebuffers->envcubemap_convolutionFBO.Bind(i);
        shaders->EnvCubeMap_ConvolutionShader.SetUniformmatri4fv("view", captureViews[i]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderer.DrawArray(vertex_arrays->cubeVa, shaders->EnvCubeMap_ConvolutionShader);
    }
    pre_framebuffers->envcubemap_convolutionFBO.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}

void Game::Generate_EnvLight_Specular()
{
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };
    shaders->EnvCubeMap_spec_ConvolutionShader.Bind();
    shaders->EnvCubeMap_spec_ConvolutionShader.SetUniformmatri4fv("projection", captureProjection);
    unsigned int maxMipLevels = 5;
    textures->skybox.Bind();
    for (unsigned int i = 0; i < maxMipLevels; ++i)
    {
        unsigned int mipWidth = 1024 * static_cast<unsigned int>(std::pow(0.5, i));
        unsigned int mipHeight = 1024 * static_cast<unsigned int>(std::pow(0.5, i));
        pre_framebuffers->envcubemap_spec_convolutionFBO.Bindmip_Renderbuffer(mipWidth, mipHeight);
        pre_framebuffers->envcubemap_spec_convolutionFBO.SetViewPort(mipWidth, mipHeight);
        float roughness = (float)i / (float)(maxMipLevels - 1);
        shaders->EnvCubeMap_spec_ConvolutionShader.SetUniform1f("roughness", roughness);
        for (int j = 0; j < 6; j++)
        {
            pre_framebuffers->envcubemap_spec_convolutionFBO.Bind(j, i);
            shaders->EnvCubeMap_spec_ConvolutionShader.SetUniformmatri4fv("view", captureViews[j]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderer.DrawArray(vertex_arrays->cubeVa, shaders->EnvCubeMap_spec_ConvolutionShader);
        }
    }
    textures->skybox.UnBind();
    pre_framebuffers->envcubemap_spec_convolutionFBO.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}

void Game::Generate_EnvLight_Specular_BRDF()
{
    shaders->EnvCubeMap_spec_BRDF_Shader.Bind();
    pre_framebuffers->envcubemap_spec_BRDF_FBO.Bind();
    pre_framebuffers->envcubemap_spec_BRDF_FBO.SetViewPort();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    renderer.DrawArray(vertex_arrays->quadVa, shaders->EnvCubeMap_spec_BRDF_Shader);
    glEnable(GL_DEPTH_TEST);
    pre_framebuffers->envcubemap_spec_BRDF_FBO.UnBind();
    glViewport(0, 0, screenWidth, screenHeight);
}



void Game::RenderText(Shader& text_shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    // �����Ӧ����Ⱦ״̬
    text_shader.Bind();
    text_shader.SetUniform3f("textColor", color);
    glm::mat4 projection = glm::ortho(0.0f, float(screenWidth), 0.0f, float(screenHeight));
    text_shader.SetUniformmatri4fv("projection", projection);
    glActiveTexture(GL_TEXTURE0);
    vertex_arrays->textVa.Bind();
    // �����ı������е��ַ�
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // ��ÿ���ַ�����VBO
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }
        };
        // ���ı����ϻ�����������
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // ����VBO�ڴ������
        vertex_buffers->textVb.Bind();
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        vertex_buffers->textVb.Unbind();
        // �����ı���
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glCheckError();
        // ����λ�õ���һ�����ε�ԭ�㣬ע�ⵥλ��1/64����
        x += (ch.Advance >> 6) * scale; // λƫ��6����λ����ȡ��λΪ���ص�ֵ (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Game::Debug()
{
    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
    glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0,
        GL_DEBUG_SEVERITY_MEDIUM, -1, "error message here");

}

std::string Game::Collision_detection()
{
    std::string text("");
    bool x_collision;
    bool y_collision;
    bool z_collision;

    for (std::unordered_map<std::string, Model*>::iterator iter = models->models_map.begin(); iter != models->models_map.end(); iter++)
    {
        x_collision = ((max(models->Main_character.aabb[1], iter->second->aabb[1]) - min(models->Main_character.aabb[0], iter->second->aabb[0]))
            < (iter->second->aabb[1] - iter->second->aabb[0] + models->Main_character.aabb[1] - models->Main_character.aabb[0]));
        y_collision = ((max(models->Main_character.aabb[3], iter->second->aabb[3]) - min(models->Main_character.aabb[2], iter->second->aabb[2]))
            < (iter->second->aabb[3] - iter->second->aabb[2] + models->Main_character.aabb[3] - models->Main_character.aabb[2]));
        z_collision = ((max(models->Main_character.aabb[5], iter->second->aabb[5]) - min(models->Main_character.aabb[4], iter->second->aabb[4]))
            < (iter->second->aabb[5] - iter->second->aabb[4] + models->Main_character.aabb[5] - models->Main_character.aabb[4]));
        if (x_collision && y_collision && z_collision)
        {
            text += std::string(iter->first);
        }
    }
    for (std::unordered_map<std::string, Model*>::iterator iter = models->anime_models_map.begin(); iter != models->anime_models_map.end(); iter++)
    {
        x_collision = ((max(models->Main_character.aabb[1], iter->second->aabb[1]) - min(models->Main_character.aabb[0], iter->second->aabb[0]))
            < (iter->second->aabb[1] - iter->second->aabb[0] + models->Main_character.aabb[1] - models->Main_character.aabb[0]));
        y_collision = ((max(models->Main_character.aabb[3], iter->second->aabb[3]) - min(models->Main_character.aabb[2], iter->second->aabb[2]))
            < (iter->second->aabb[3] - iter->second->aabb[2] + models->Main_character.aabb[3] - models->Main_character.aabb[2]));
        z_collision = ((max(models->Main_character.aabb[5], iter->second->aabb[5]) - min(models->Main_character.aabb[4], iter->second->aabb[4]))
            < (iter->second->aabb[5] - iter->second->aabb[4] + models->Main_character.aabb[5] - models->Main_character.aabb[4]));
        if (x_collision && y_collision && z_collision)
        {
            text += std::string(iter->first);
        }
    }
    return text;
}

void Game::Update_Spotlight()
{
    glm::vec3 right = glm::cross(camera.Front, glm::vec3(0.0f, 1.0f, 0.0f));
    spotlight.bias_direction = glm::vec3(0.15f) * (right + camera.Front);
    spotlight.position = camera.Position + spotlight.bias_direction;
    spotlight.direction = camera.Front - spotlight.bias_direction;
    spotlight.far_plane = keyinput.spot_far_plane;
    glm::mat4 SpotlightProjection = glm::perspective(glm::radians(45.0f), 1.0f, spotlight.near_plane, spotlight.far_plane);
    glm::mat4 SpotlightView = glm::lookAt(spotlight.position, spotlight.position + spotlight.direction, glm::vec3(0.0f, 1.0f, 0.0f));//����Ϊ���Ĺ��շ���
    spotlight.SpotlightSpaceMatrix = SpotlightProjection * SpotlightView;
}


void Game::Generate_Particle()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    Shader& shader = shaders->Particle_shader;
    shader.Bind();
    glm::mat4 projection = glm::ortho(0.0f, float(screenWidth), 0.0f, float(screenHeight));
    shader.SetUniformmatri4fv("projection", projection);
    shader.SetUniform1f("scale", keyinput.particle_scale);
    textures->Particle_texture.Bind();
    for (Particle particle : particle_generator.particles)
    {
        if (particle.Life > 0.0f)
        {
            shader.SetUniform2f("offset", particle.Position);
            shader.SetUniform4f("color", particle.Color);
            renderer.DrawArray(vertex_arrays->particleVa, shader);
        }
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC1_ALPHA);
}

void Game::Generate_D3Particle()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    Shader& shader = shaders->D3Particle_shader;
    shader.Bind();
    shader.SetUniformmatri4fv("view", camera.GetViewMatrix());
    shader.SetUniformmatri4fv("projection", camera.GetProjectionMatrix());
    textures->floor_diffuse.Bind();
    for (D3Particle& particle : d3particle_generator.particles)
    {
        if (particle.Life > 0.0f)
        {
            shader.SetUniformmatri4fv("model", particle.Position);
            shader.SetUniform4f("color", particle.Color);
            renderer.DrawElement(*models->Sphere.va, *models->Sphere.ib, shader);
        }
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC1_ALPHA);
}

void Game::Initialize_Terrain_gpu()
{
    terrain = Get_TerrainData_gpu(textures->Terrain_texture.Get_width(), textures->Terrain_texture.Get_height());
    //models->Terrain.Get_index(&terrain.index[0], static_cast<unsigned int>(terrain.index.size()));
    VertexBuffer terrainVb(&terrain.vertex[0], sizeof(float) * static_cast<unsigned int>(terrain.vertex.size()));
    VertexBufferLayout terrainLayout;
    terrainLayout.Push<float>(3);//position
    terrainLayout.Push<float>(2);
    models->Terrain.va->AddBuffer(terrainVb, terrainLayout);

}

void Game::Generate_Terrain_gpu()
{

    Shader& shader = shaders->Terrain_gpu_shader;
    shader.Bind();
    // view/projection transformations
    glm::mat4 projection = camera.GetProjectionMatrix();
    glm::mat4 view = camera.GetViewMatrix();
    shader.SetUniformmatri4fv("projection", projection);
    shader.SetUniformmatri4fv("view", view);


    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    //model[3][1] = -10.0f;
    shader.SetUniformmatri4fv("model", model);
    textures->Terrain_texture.Bind(0);
    shader.SetUniform1i("heightMap", 0);
    textures->floor_diffuse.Bind(1);
    shader.SetUniform1i("textureMap", 1);
    shader.SetUniform1i("tess_level", keyinput.tess_level);
    // render the cube
    models->Terrain.va->Bind();
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_PATCHES, 0, 4 * terrain.rez * terrain.rez);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    textures->floor_diffuse.UnBind();
    models->Terrain.va->Unbind();
    shader.UnBind();
}

void Game::Generate_Health_bar()
{
    Shader& shader = shaders->Health_bar_shader;
    shader.Bind();
    shader.SetUniform1f("screenHeight", static_cast<float>(screenHeight));

    shader.SetUniform1f("max_life", my_state.max_life);
    shader.SetUniform1f("current_life", my_state.current_life);
    shader.SetUniform1f("max_magic", my_state.max_magic);
    shader.SetUniform1f("current_magic", my_state.current_magic);
    shader.SetUniform1f("max_energy", my_state.max_energy);
    shader.SetUniform1f("current_energy", my_state.current_energy);
    renderer.DrawArray(vertex_arrays->quadVa, shader);
    Generate_Health_bar_enemy();
}

void Game::Generate_Health_bar_enemy()
{
    Shader& shader = shaders->Health_bar_enemy_shader;
    shader.Bind();
    std::map<float, std::string> sorted;
    for (std::unordered_map<std::string, Model*>::iterator iter = models->anime_models_map.begin(); iter != models->anime_models_map.end(); iter++)
    {
        if (iter->first != "Main_character")
        {
            float distance = glm::length(camera.Position - glm::vec3(iter->second->position.GetVector(3, 0), iter->second->position.GetVector(3, 1), iter->second->position.GetVector(3, 2)));
            sorted[distance] = iter->first;
        }
    }
    csm_dirlight.Set_far_plane(sorted.rbegin()->first * 10);
    for (std::map<float, std::string>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
    {
        Model& model = *models->anime_models_map[it->second];
        float health_bar_x = (model.aabb[min_x] + model.aabb[max_x]) * 0.5f;
        float health_bar_y = model.aabb[max_y] + (model.aabb[max_y] - model.aabb[min_y]) * 0.1f;
        float health_bar_z = (model.aabb[min_z] + model.aabb[max_z]) * 0.5f;
        glm::vec4 health_bar_position = camera.GetProjectionMatrix() * camera.GetViewMatrix() * glm::vec4(health_bar_x, health_bar_y, health_bar_z, 1.0f);
        float life_x = ((health_bar_position.x / health_bar_position.w * 0.5f) + 0.5f) * screenWidth;
        float life_y = ((health_bar_position.y / health_bar_position.w * 0.5f) + 0.5f) * screenHeight;
        shader.SetUniform1f("life_y", life_y);
        shader.SetUniform1f("life_x", life_x);

        shader.SetUniform1f("max_life", model.max_life);
        shader.SetUniform1f("current_life", model.current_life);

        glm::vec3 viewPos;
        if (keyinput.third_view)
            viewPos = camera.Get_third_position();
        else
            viewPos = camera.Get_first_position();
        glm::vec3 FragPos = glm::vec3(health_bar_x, health_bar_y, health_bar_z);
        glm::vec3 viewDir = glm::normalize(FragPos - viewPos);
        float length = glm::length(FragPos - viewPos);
        float theta = dot(viewDir, camera.Front);
        if (theta > 0.707 && length < 20.0f)
            renderer.DrawArray(vertex_arrays->quadVa, shader);
    }
}

void Game::ready_render()
{
    shaders = new Shaders;
    textures = new Textures;
    models = new Models;
    pre_framebuffers = new Pre_FrameBuffers();
    framebuffers = new FrameBuffers(screenWidth, screenHeight);
    vertex_arrays = new VertexArrays;
    vertex_buffers = new VertexBuffers;
    animations = new Animations(&models->Robot_boxing);
    Debug();
    for (std::unordered_map<std::string, Animator>::iterator iter = models->animator_map.begin(); iter != models->animator_map.end(); iter++)
    {
        iter->second.PlayAnimation(animations->Animation_walk);
        iter->second.UpdateAnimation(deltaTime);
    }
    /*thread t1(&Game::read_config, this);
    
    thread t2(&Game::Initialize_Models_Positions, this);
    thread t3(&Game::Initialize_Pointlight, this);
    t1.join();
    t2.join();
    t3.join();*/
    read_config();
    Initialize_Models_Positions();
    Initialize_Pointlight();
    Initialize_Vertex_Arrays();
    Initialize_Terrain_gpu();
    TextFBO::Init();
    glCheckError();
    Generate_CubeTexture();
    glCheckError();

    Generate_EnvLight_Diffuse();
    Generate_EnvLight_Specular();
    Generate_EnvLight_Specular_BRDF();
    glCheckError();


    glCheckError();
    Generate_Instance(*models->models_map["Rock"], 1000);
    
    for (std::unordered_map<std::string, Model*>::iterator iter = models->anime_models_map.begin(); iter != models->anime_models_map.end(); iter++)
    {
        octree.update(iter->second);
    }
}
void Game::ProcessAction()
{
    if (camera.is_move && !response_action)
    {
        models->animator_map["Main_character"].UpdateAnimation(deltaTime);
    }
    if (keyinput.chage_animation)
    {
        for (std::unordered_map<std::string, Animator>::iterator iter = models->animator_map.begin(); iter != models->animator_map.end(); iter++)
        {
            iter->second.PlayAnimation(animations->animations_map[keyinput.animation_type]);
        }
        keyinput.chage_animation = false;
        response_action = true;
    }
    if (response_action)
    {
        models->animator_map["Main_character"].UpdateAnimation(deltaTime);
        if (models->animator_map["Main_character"].GetCurrentTime() > models->animator_map["Main_character"].GetDuration() * 0.95f)
        {
            models->animator_map["Main_character"].PlayAnimation(animations->animations_map[animations->walk]);
            models->animator_map["Main_character"].UpdateAnimation(deltaTime);
            response_action = false;
        }
    }
}
void Game::start_render()
{
    //
    ProcessAction();
    camera.Set_third_view(keyinput.third_view);
    camera.Set_free_view(keyinput.free_view);
    glEnable(GL_DEPTH_TEST);
    Update_Pointlight();
    Update_Models_Positions();
    Update_Sun();
    csm_dirlight.Get_light_projection(camera, sun.Sun_Position);

    Generate_Dir_CSM();
    //�����Ӱ��ͼ
    if (pointlight.lightintensity > 0)
        Generate_Point_SM();

    //�۹���Ӱ��ͼ
    Update_Spotlight();
    Generate_Spot_SM();

    //defered shading ����Gbuffer   
    Generate_Defered_basicDATA();

    framebuffers->gbuffer.Read();
    framebuffers->cameradepthFBO.Write();
    framebuffers->gbuffer.BlitDepthBuffer();
    framebuffers->gbuffer.UnBind();

    if (keyinput.useSSAO)
    {
        Generate_SSAO();
        //blur
        Generate_SSAO_blur();
    }
    Generate_PreShadow();

    //����uniform      ��ɫ
    Generate_Origin_Screen();
    //���Դģ�� 
    //��ȡgbuffer�������Ϣ�Խ��������Ⱦ    
    Generate_SkyBox();

    //���ڴ���
    Generate_PostProcess();

    Generate_TextParticle();
    Generate_Health_bar();
}

void Game::Generate_TextParticle()
{
    glEnable(GL_BLEND);
    std::string text_render = Collision_detection();
    RenderText(shaders->text_shader, text_render, 0.0f, 600.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    if (keyinput.show_particle)
    {
        particle_generator.update_Particle(glm::vec2(screenWidth / 2, 0), keyinput.particle_offset, keyinput.new_particle_num, keyinput.particle_vel, keyinput.particle_life_reduce / 10);
        Generate_Particle();

    }
    if (glfwGetTime() < 10)
    {
        RenderText(shaders->text_shader, "welcome to the demo", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    }
    RenderText(shaders->text_shader, "x: " + std::to_string(camera.Position.x), screenWidth * 0.95f, screenHeight * 0.95f, 0.5f, glm::vec3(1.0f));
    RenderText(shaders->text_shader, "y: " + std::to_string(camera.Position.y), screenWidth * 0.95f, screenHeight * 0.93f, 0.5f, glm::vec3(1.0f));
    RenderText(shaders->text_shader, "z: " + std::to_string(camera.Position.z), screenWidth * 0.95f, screenHeight * 0.91f, 0.5f, glm::vec3(1.0f));
    RenderText(shaders->text_shader, "FPS: " + std::to_string(int(1.0f / deltaTime)), screenWidth * 0.93f, 0, 0.5f, glm::vec3(1.0f));

    glDisable(GL_BLEND);
}
void Game::attack()
{
    if (!response_action)
    {
        if (keyinput.keysPressed[GLFW_KEY_LEFT_SHIFT])
        {    //�ػ�
            models->animator_map["Main_character"].PlayAnimation(animations->animations_map[animations->attack]);
        }
        else
            models->animator_map["Main_character"].PlayAnimation(animations->animations_map[animations->punching]);
        response_action = true;
    }
}

void Generate_Instance(Model& m, int amount)
{
    //unsigned int amount = 10000;
    std::vector<glm::mat4> modelMatrices(amount);
    std::srand(glfwGetTime()); // ��ʼ���������    
    float radius = 75.0;
    float offset = 15.0f;
    for (unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        // 1. λ�ƣ��ֲ��ڰ뾶Ϊ 'radius' ��Բ���ϣ�ƫ�Ƶķ�Χ�� [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // �����Ǵ��ĸ߶ȱ�x��z�Ŀ���ҪС
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. ���ţ��� 0.05 �� 0.25f ֮������
        float scale = (rand() % 20) / 100.0f + 0.05;
        model = glm::scale(model, glm::vec3(scale));

        // 3. ��ת������һ�����룩���ѡ�����ת�����������������ת
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. ���ӵ������������
        modelMatrices[i] = model;
    }

    VertexBuffer Vb(&modelMatrices[0], modelMatrices.size() * sizeof(glm::mat4));
    for (unsigned int i = 0; i < m.meshes.size(); i++)
    {
        unsigned int VAO = m.meshes[i].VAO;
        glBindVertexArray(VAO);
        // ��������
        GLsizei vec4Size = sizeof(glm::vec4);
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
        glEnableVertexAttribArray(8);
        glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
        glEnableVertexAttribArray(9);
        glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
        glEnableVertexAttribArray(10);
        glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

        glVertexAttribDivisor(7, 1);
        glVertexAttribDivisor(8, 1);
        glVertexAttribDivisor(9, 1);
        glVertexAttribDivisor(10, 1);
        glBindVertexArray(0);
    }
}

void Draw_Instance(Model& m, int amount, Shader& shader)
{
    for (unsigned int i = 0; i < m.meshes.size(); i++)
    {
        Mesh& mesh = m.meshes[i];
        glBindVertexArray(m.meshes[i].VAO);
        for (unsigned int j = 0; j < mesh.textures.size(); j++)
        {
            glActiveTexture(GL_TEXTURE0 + j); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            std::string name = mesh.textures[j].type;

            // now set the sampler to the correct texture unit
            if(!name.empty())
                shader.SetUniform1i(("material." + name).c_str(), j);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, mesh.textures[j].id);
        }
        glDrawElementsInstanced(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0, amount);
    }
}
bool Game::isInFrustum(std::vector<float>& aabb)
{
    std::vector<glm::vec3> points = csm_dirlight.GetCameraFrustum(0);
    glm::vec3 center = glm::vec3((aabb[min_x] + aabb[max_x]) * 0.5, (aabb[min_y] + aabb[max_y]) * 0.5, (aabb[min_z] + aabb[max_z]) * 0.5);
    glm::vec3 extents = glm::vec3((aabb[max_x] - aabb[min_x]) * 0.5, (aabb[max_y] - aabb[min_y]) * 0.5, (aabb[max_z] - aabb[min_z]) * 0.5);
    auto f = [&](glm::vec3 normal, glm::vec3 Po)->bool
    {
        float distance = glm::dot(normal, center - Po);
        float m_distance = extents.x * abs(normal.x) + extents.y * abs(normal.y) + extents.z * abs(normal.z);
        return -m_distance <= distance;
    };
    bool conditions[6]{};
    //left
    glm::vec3 normal = glm::normalize(-glm::cross(points[1] - points[0], points[4] - points[0]));
    Entity::Plane pLeft = Entity::Plane(normal, points[0]);
    conditions[0] = f(pLeft.normal, pLeft.Po);

    //right
    normal = glm::normalize(glm::cross(points[2] - points[3], points[7] - points[3]));
    Entity::Plane pRight = Entity::Plane(normal, points[3]);
    conditions[1] = f(pRight.normal, pRight.Po);

    //up
    normal = glm::normalize(-glm::cross(points[2] - points[1], points[5] - points[1]));
    Entity::Plane pUp = Entity::Plane(normal, points[1]);
    conditions[2] = f(pUp.normal, pUp.Po);

    //down
    normal = glm::normalize(glm::cross(points[3] - points[0], points[4] - points[0]));
    Entity::Plane pDown = Entity::Plane(normal, points[0]);
    conditions[3] = f(pDown.normal, pDown.Po);

    //forward
    Entity::Plane pForward = Entity::Plane(camera.Front, points[0]);
    conditions[4] = f(pForward.normal, pForward.Po);

    //back
    Entity::Plane pBack = Entity::Plane(-camera.Front, points[0] + camera.Front * camera.far_plane);
    conditions[5] = f(pBack.normal, pBack.Po);

    return conditions[0] && conditions[1] && conditions[2] && conditions[3] && conditions[4] && conditions[5];
}
bool Game::isInFrustum(glm::vec3& pointMin, glm::vec3& pointMax)
{
    std::vector<float> a =
    {
        pointMin.x,
        pointMax.x,
        pointMin.y,
        pointMax.y,
        pointMin.z,
        pointMax.z
    };
    return isInFrustum(a);
}

void Game::cutOffFrustum()
{
    for (std::unordered_map<std::string, Model*>::iterator iter = models->models_map.begin(); iter != models->models_map.end(); iter++)
    {
        iter->second->inFrustum = isInFrustum(iter->second->aabb);
    }
    for (std::unordered_map<std::string, Model*>::iterator iter = models->anime_models_map.begin(); iter != models->anime_models_map.end(); iter++)
    {
        iter->second->inFrustum = isInFrustum(iter->second->aabb);
    }
}
void Game::drawModels(Shader& shader)
{
    cutOffFrustum();
    for (std::unordered_map<std::string, Model*>::iterator iter = models->models_map.begin(); iter != models->models_map.end(); iter++)
    {
        if (iter->second->inFrustum)
        {
            shader.SetUniformmatri4fv("model", iter->second->position.GetModelSpace());
            iter->second->Draw(shader);
        }
    }
    ////����map��ÿ��model��׶���ཻ����
    ////��׶���޳�
    //
    for (std::unordered_map<std::string, Model*>::iterator iter = models->anime_models_map.begin(); iter != models->anime_models_map.end(); iter++)
    {      
        if (iter->second->inFrustum)
        {
            auto transforms = models->animator_map[iter->first].GetFinalBoneMatrices();
            for (int i = 0; i < transforms.size(); ++i)
            {
                shader.SetUniformmatri4fv("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
            }
            shader.SetUniformmatri4fv("model", iter->second->position.GetModelSpace());
            iter->second->Draw(shader);
        }
    }
    //�˲����ཻ����
    //cutList.clear();//�˲����޳�
    //cutOffTree(octree);
    //for (std::unordered_map<std::string, Model*>::iterator iter = models->anime_models_map.begin(); iter != models->anime_models_map.end(); iter++)
    //{       
    //    if (iter->second->isMoved)
    //    {
    //        octree.update(iter->second);
    //        iter->second->isMoved = false;
    //    }
    //    if (std::find(cutList.begin(), cutList.end(), iter->second) == cutList.end())
    //    {
    //        shader.SetUniformmatri4fv("model", iter->second->position.GetModelSpace());
    //        auto transforms = models->animator_map[iter->first].GetFinalBoneMatrices();
    //        for (int i = 0; i < transforms.size(); ++i)
    //        {
    //            shader.SetUniformmatri4fv("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
    //        }
    //        iter->second->Draw(shader);
    //    }
    //}
}

void Game::cutOffTree(Entity::Octree& o)
{
    if (!isInFrustum(o.pointMin, o.pointMax))
    {
        for (std::unordered_map<Model*, int>::iterator it = o.hasModel.begin(); it != o.hasModel.end(); it++)
        {
            cutList.push_back(it->first);
        }
    }
    else
    {
        if (o.children.empty())
        {
            for (std::unordered_map<Model*, int>::iterator it = o.hasModel.begin(); it != o.hasModel.end(); it++)
            {
                if(!isInFrustum(it->first->aabb))
                    cutList.push_back(it->first);
            }
        }
        else
        {
            for (std::unordered_map<Model*, int>::iterator it = o.hasModel.begin(); it != o.hasModel.end(); it++)
            {
                if (it->second == 8 && !isInFrustum(it->first->aabb))
                    cutList.push_back(it->first);
            }
            for (Entity::Octree& tree : o.children)
            {
                cutOffTree(tree);
            }
        }
    }

}
void Game::drawModelsShadow(Shader& shader)
{
    for (std::unordered_map<std::string, Model*>::iterator iter = models->models_map.begin(); iter != models->models_map.end(); iter++)
    {
        shader.SetUniformmatri4fv("model", iter->second->position.GetModelSpace());
        if (iter->second->inFrustum)
            iter->second->DrawShadow(shader);
    }
    for (std::unordered_map<std::string, Model*>::iterator iter = models->anime_models_map.begin(); iter != models->anime_models_map.end(); iter++)
    {
        auto transforms = models->animator_map[iter->first].GetFinalBoneMatrices();
        for (int i = 0; i < transforms.size(); ++i)
        {
            shader.SetUniformmatri4fv("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        }
        shader.SetUniformmatri4fv("model", iter->second->position.GetModelSpace());
        if (iter->second->inFrustum)
            iter->second->DrawShadow(shader);
    }
}

