#include <iostream>
#include <vector>

#include "CGL/vector2D.h"

#include "mass.h"
#include "rope.h"
#include "spring.h"

namespace CGL {

    Rope::Rope(Vector2D start, Vector2D end, int num_nodes, float node_mass, float k, vector<int> pinned_nodes)
    {
        // TODO (Part 1): Create a rope starting at `start`, ending at `end`, and containing `num_nodes` nodes.
        Vector2D node_pos = start;
        Vector2D offset = (end - start)/(num_nodes-1);
        //创建结点。
        for(int i = 0;i < num_nodes;i++){
            masses.push_back(new Mass(node_pos,node_mass,false));
            node_pos += offset;
        }
       // Comment-in this part when you implement the constructor
        for (auto &i : pinned_nodes) {  //设置第一个结点固定。
            masses[i]->pinned = true;
        }
        //在两个结点之间创建弹簧。传入结点index和弹簧系数。n个结点有n-1段spring。
        for(int i = 0;i < num_nodes - 1;i++){
            springs.push_back(new Spring(masses[i],masses[i + 1],k));
        }

    }

    void Rope::simulateEuler(float delta_t, Vector2D gravity)
    {
        float kd;
        for (auto &s : springs)
        {
            //应用胡克定律对每个结点计算力。
            Mass * l,* r;
            l = s->m1;
            r = s->m2;
            l->forces += s->k * ((r->position - l->position).norm() - s->rest_length) * (r->position - l->position).unit();
            r->forces += s->k * ((l->position - r->position).norm() - s->rest_length) * (l->position - r->position).unit();
            // TODO (Part 2): 增加阻尼
            l->forces += 0.1 * (l->position - r->position).unit() * (l->velocity - r->velocity) * (l->position - r->position).unit();
            r->forces += 0.1 * (r->position - l->position).unit() * (r->velocity - l->velocity) * (r->position - l->position).unit();
        }

        Vector2D a;
        Vector2D G;
        for (auto &m : masses)
        {
            if (!m->pinned)
            {
                G = m->mass *  gravity;
                // TODO (Part 2): 加入重力。计算新的速度和位置
                a = (m->forces + G) / m->mass;
                m->velocity = m->velocity + a * delta_t;
                m->position = m->position + m->velocity * delta_t;
            }
            // Reset all forces on each mass
            m->forces = Vector2D(0, 0);
        }
    }

    void Rope::simulateVerlet(float delta_t, Vector2D gravity)
    {
        for (auto &s : springs)
        {
            Mass * l,* r;
            l = s->m1;
            r = s->m2;
            l->forces += s->k * ((r->position - l->position).norm() - s->rest_length) * (r->position - l->position).unit();
            r->forces += s->k * ((l->position - r->position).norm() - s->rest_length) * (l->position - r->position).unit();
        }

        Vector2D a;
        Vector2D G;
        for (auto &m : masses)
        {
            if (!m->pinned)
            {
                Vector2D temp_position = m->position;
                // TODO (Part 3.1): Set the new position of the rope mass
                // TODO (Part 4): Add global Verlet damping
                G = m->mass *  gravity;
                a = (m->forces + G) / m->mass;
                m->velocity = m->velocity + a * delta_t;
                m->position = m->position + (1 - 0.0001) * (m->position - m->last_position) +  a * delta_t * delta_t;
                m->last_position = temp_position;
            }
            m->forces = Vector2D(0, 0);
        }
    }
}
