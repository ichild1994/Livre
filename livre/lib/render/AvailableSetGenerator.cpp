/* Copyright (c) 2011-2015, EPFL/Blue Brain Project
 *                          Ahmet Bilgili <ahmet.bilgili@epfl.ch>
 *
 * This file is part of Livre <https://github.com/BlueBrain/Livre>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <livre/core/dash/DashRenderNode.h>
#include <livre/core/dash/DashRenderStatus.h>
#include <livre/core/dash/DashTree.h>
#include <livre/core/render/RenderBrick.h>
#include <livre/core/render/View.h>
#include <livre/core/data/VolumeDataSource.h>
#include <livre/core/data/VolumeInformation.h>
#include <livre/core/visitor/RenderNodeVisitor.h>
#include <livre/lib/render/AvailableSetGenerator.h>
#include <livre/lib/visitor/CollectionTraversal.h>
#include <livre/lib/visitor/DFSTraversal.h>
#include <livre/lib/cache/TextureObject.h>

namespace livre
{

typedef boost::unordered_map< NodeId, dash::NodePtr > NodeIdDashNodeMap;

class VisibleCollectorVisitor : public RenderNodeVisitor
{
public:

    VisibleCollectorVisitor( DashTreePtr dashTree,
                             DashNodeVector& nodeVector );

    void visit( DashRenderNode& renderNode, VisitState& state ) final;
    DashNodeVector& nodeVector_;
};

VisibleCollectorVisitor::VisibleCollectorVisitor(
        DashTreePtr dashTree,
        DashNodeVector& nodeVector )
    : RenderNodeVisitor( dashTree ),
      nodeVector_( nodeVector )
{ }

void VisibleCollectorVisitor::visit( DashRenderNode& renderNode, VisitState& state )
{
    const LODNode& lodNode = renderNode.getLODNode();
    if( !lodNode.isValid( ))
        return;

    if( !renderNode.isInFrustum( ))
    {
        state.setVisitChild( false );
        return;
    }

    if( renderNode.isVisible( ))
    {
        nodeVector_.push_back( renderNode.getDashNode() );
        state.setVisitChild( false );
    }
 }

class LoadedTextureCollectVisitor : public RenderNodeVisitor
{
public:

    LoadedTextureCollectVisitor( DashTreePtr dashTree,
                                 NodeIdDashNodeMap& nodeIdDashNodeMap,
                                 DashNodeVector& notAvailableRenderNodeList )
        : RenderNodeVisitor( dashTree ),
          _nodeIdDashNodeMap( nodeIdDashNodeMap ),
          _notAvailableRenderNodeList( notAvailableRenderNodeList )
    {}

    void visit( DashRenderNode& renderNode, VisitState& ) final
    {
        dash::NodePtr current = renderNode.getDashNode();
        while( current )
        {
            DashRenderNode currentRenderNode( current );
            const NodeId& nodeId = currentRenderNode.getLODNode().getNodeId();

            const ConstTextureObjectPtr texture =
                boost::static_pointer_cast< const TextureObject >(
                        currentRenderNode.getTextureObject( ));

            if( texture && texture->isLoaded( ))
            {
                _nodeIdDashNodeMap[ nodeId ] = current;
                break;
            }

            current = nodeId.isRoot() ? 0 : getDashTree()->getParentNode( nodeId );
        }

        if( renderNode.getDashNode() != current )
            _notAvailableRenderNodeList.push_back( renderNode.getDashNode() );
    }

private:
    NodeIdDashNodeMap& _nodeIdDashNodeMap;
    DashNodeVector& _notAvailableRenderNodeList;
};


bool hasParentInMap( DashRenderNode& childRenderNode,
                     const NodeIdDashNodeMap& nodeIdDashNodeMap )
{
    const LODNode& childNode = childRenderNode.getLODNode();

    const NodeId childNodeId = childNode.getNodeId();
    const NodeIds& parentNodeIds = childNodeId.getParents();

    BOOST_FOREACH( const NodeId& parentId, parentNodeIds )
    {
        if( nodeIdDashNodeMap.find( parentId ) != nodeIdDashNodeMap.end() )
            return true;
    }
    return false;
}

AvailableSetGenerator::AvailableSetGenerator( DashTreePtr tree )
    : RenderingSetGenerator( tree )
{
}

void AvailableSetGenerator::generateRenderingSet( const Frustum&,
                                                  FrameInfo& frameInfo )
{
    VisibleCollectorVisitor visibleSelector( getDashTree(),
                                             frameInfo.allNodesList );
    DFSTraversal dfsTraverser_;
    dfsTraverser_.traverse( getDashTree()->getDataSource()->getVolumeInformation().rootNode,
                            visibleSelector, getDashTree()->getRenderStatus().getFrameID( ));

    NodeIdDashNodeMap nodeIdDashNodeMap;
    LoadedTextureCollectVisitor collector( getDashTree(),
                                           nodeIdDashNodeMap,
                                           frameInfo.notAvailableRenderNodeList );

    CollectionTraversal colTraverser;
    colTraverser.traverse( frameInfo.allNodesList, collector );

    if( !frameInfo.notAvailableRenderNodeList.empty( ))
    {
        NodeIdDashNodeMap::const_iterator it = nodeIdDashNodeMap.begin();
        size_t previousMapSize = 0;
        do
        {
            previousMapSize = nodeIdDashNodeMap.size();
            while( it != nodeIdDashNodeMap.end( ))
            {
                DashRenderNode childNode( it->second );
                if( hasParentInMap( childNode, nodeIdDashNodeMap ))
                    it = nodeIdDashNodeMap.erase( it );
                else
                    ++it;
            }
        }
        while( previousMapSize != nodeIdDashNodeMap.size( ));
    }

    frameInfo.renderNodeList.reserve( nodeIdDashNodeMap.size( ));
    for( NodeIdDashNodeMap::const_iterator it = nodeIdDashNodeMap.begin();
         it != nodeIdDashNodeMap.end(); ++it )
    {
        frameInfo.renderNodeList.push_back( it->second );
    }
}

}