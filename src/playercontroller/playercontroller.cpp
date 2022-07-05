//
// Created by Amo on 2022/7/5.
//
#include "playercontroller/playercontroller.h"
#include "core.h"
#include "core/constants.h"
#include "core/window.h"
#include "core/ECS/registry.h"
#include "core/ECS/component.h"
#include "core/ECS/Systems/physics_system.h"
#include "world/world.h"
#include "world/chunk_manager.h"

void SymoCraft::PlayerController::DoRayCast( ECS::Registry &registry, Window &window)
{
    Character::CharacterComponent controller = registry.GetComponent<Character::CharacterComponent>(World::GetPlayer());
    Character::PlayerComponent player_com = registry.GetComponent<Character::PlayerComponent>(World::GetPlayer());
    Transform &transform = registry.GetComponent<Transform>(World::GetPlayer());

    if (glfwGetKey((GLFWwindow*)window.window_ptr, GLFW_KEY_P) == GLFW_PRESS)  // just for debug
        printf("transform front is (%f, %f, %f)\n", transform.front.x, transform.front.y, transform.front.z);


    RaycastStaticResult res = Physics::RayCastStatic(transform.position + player_com.camera_offset, transform.front, 3.0f);
    if (res.hit)
    {
        //printf("ray hitted\n");
        glm::vec3 block_looking_atpos = res.point - (res.hit_normal * 0.1f);

        if (glfwGetMouseButton((GLFWwindow*)window.window_ptr, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS
            && Application::block_place_debounce <=0)
        {
            Block new_block = { 0, 0, 0, 0};
            new_block.block_id = Application::new_block_id;

            if (new_block != BlockConstants::NULL_BLOCK && new_block != BlockConstants::AIR_BLOCK)
            {
                glm::vec3 world_pos = res.point + (res.hit_normal * 0.1f);
                ChunkManager::SetBlock(world_pos, new_block.block_id);
            }
            Application::block_place_debounce = Application::kBlockPlaceDebounceTime;
        }
        else
        if (glfwGetMouseButton((GLFWwindow*)window.window_ptr , GLFW_MOUSE_BUTTON_LEFT)  == GLFW_PRESS
            && Application::block_place_debounce <= 0)
        {
            static int num_delete;
            glm::vec3 worldPos = res.point - (res.hit_normal * 0.1f);
            ChunkManager::RemoveBLock(worldPos);
            num_delete++;
            std::cout << "block removed " << num_delete  << std::endl;
            Application::block_place_debounce = Application::kBlockPlaceDebounceTime;
        }
    }
}