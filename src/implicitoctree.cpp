#include "implicitoctree.h"
#include "marchingcube.h"

void ImplicitOctree::checkDistanceOnNode(TreeNode * node)
{
    if(node->m_height !=1)
        {
            for(int i=0;i<8;++i)
            {
                checkDistanceOnNode(node->m_child[i]);
            }
        }
        else
        {
            if(node->m_objectList.size()<=1)
            {
                return;
            }

// iterate through vertices and find min distance




//            glm::vec3 P, Q, Vp, N, force;
//            float Rp, Rq, dist, speed;
//            BOOST_FOREACH(Particle *currentParticle,node->m_objectList)
//            {
//                P = currentParticle->getPosition();
//                Vp = currentParticle->getCurrentSpeed();
//                speed = Vp.length();
//                Rp = currentParticle->getRadius();
//                force.m_x = force.m_y = force.m_z = 0.0;

//                BOOST_FOREACH(Particle *testParticle,node->m_objectList )
//                {
//                    // no need to self test
//                    if(testParticle==currentParticle)
//                    {
//                      // continue
//                      continue;
//                    }
//                    Q = testParticle->getPosition();
//                    Rq = testParticle->getRadius();
//                    dist = Rq + Rq - (P-Q).length();
//                    if(dist > 0) // collision between P and Q
//                    {
//                      N = P-Q;
//                      N.normalize();
//                      force += dist*N;
//                    }
//                }
//                Vp += force;
//                Vp.normalize();
//                currentParticle->setSpeed(Vp*speed);
//            }
        }

}
