//
//  Map.cpp
//

#include "Map.h"

#include <cassert>
#include <string>

//additional header files for function calls
#include "GetGlut.h"
#include "ObjLibrary/DisplayList.h"
#include "BlockMap.h"
#include "SnowMap.h"

using namespace std;
using namespace ObjLibrary;



Map :: Map ()
    // Constructor initializer list: initialize member variables
    : block_map(),
      block_list(),
      snow_map()
{
    // Assertion to ensure that the map is not initialized already
    assert(!isInitialized());  // Ensure the map is initialized
}

// Constructor for Map with map filename, block display list, and snow texture filename
Map :: Map (int size_x_in, int size_z_in,
            const DisplayList& block_display_list,
            const string& snow_texture_filename)
		: block_map(size_x_in, size_z_in),
		  block_list(block_display_list)
		  // snow_map is initialized below
{
	initSnowMap(snow_texture_filename);

	assert(isInitialized());
}

Map :: Map (const string& map_filename,
            const DisplayList& block_display_list,
            const string& snow_texture_filename)
		: block_map(map_filename),
		  block_list(block_display_list)
		  // snow_map is initialized below
{
	initSnowMap(snow_texture_filename);

	assert(isInitialized());
}


// Check if the map is initialized
bool Map :: isInitialized () const
{
	return snow_map.isSetAll();
}

// Get size of the map in the X direction
// Precondition:
// - map must be initialized
// - Block map and snow map must have the same size in the X direction
int Map :: getSizeX () const
{
    // Ensure that the map is initialized
    assert(isInitialized());
    
    // Ensure that the block map and snow map have the same size in the X direction
    assert(block_map.getSizeX() == snow_map.getSizeX());
    
    // Return the size of the map in the X direction
    return block_map.getSizeX();
}

// Get size of the map in the Z direction
// Precondition:
// - map must be initialized
// - Block map and snow map must have the same size in the Z direction
int Map :: getSizeZ () const
{
    // Ensure that the map is initialized
    assert(isInitialized());

    // Ensure that the block map and snow map have the same size in the Z direction
    assert(block_map.getSizeZ() == snow_map.getSizeZ());
    
    // Return the size of the map in the Z direction
    return block_map.getSizeZ();
}

// Check if a position is within the map
bool Map :: isInMap (int x, int z) const
{
    // Ensure that the map is initialized
        assert(isInitialized());
        
        // Ensure that both block map and snow map agree whether the position is within the map
        assert(block_map.isInMap(x, z) == snow_map.isInMap(x, z));

        // Return the result of the block map's isInMap function
        return block_map.isInMap(x, z);
}


// Get the block map
const BlockMap& Map :: getBlockMap () const
{
    // Ensure that the map is initialized
        assert(isInitialized());
        // Return the block map
        return block_map;
}

// Get the starting position for the player
Vector3 Map :: getPlayerStart (double player_half_height) const
{
	assert(isInitialized());

	return block_map.getPlayerStart(player_half_height);
}

// Get the starting position for the opponent
Vector3 Map :: getOpponentStart (double player_half_height) const
{
	assert(isInitialized());

	return block_map.getOpponentStart(player_half_height);
}

unsigned int Map :: getBlockHeight (int x, int z) const
{
    // Ensure that the map is initialized
    assert(isInitialized());
    // Check if the position is within the map
    if(isInMap(x, z))
        // If the position is within the map, return the height of the block at that position
        return block_map.getAt(x, z);
    else
        // If the position is not within the map, return 0
        return 0;
}

// Get the neighborhood of a given position
Neighbourhood Map :: getNeighbourhood (int center_x, int center_z) const
{
	assert(isInitialized()); // Ensure the map is initialized

	return Neighbourhood(block_map, center_x, center_z);
}

// Get the large noise field
const PerlinNoiseField& Map :: getNoiseLarge () const
{
	assert(isInitialized());// Ensure the map is initialized

	return noise_large;
}

// Get the small noise field
const PerlinNoiseField& Map :: getNoiseSmall () const
{
	assert(isInitialized()); // Ensure the map is initialized

	return noise_small;
}

const SnowMap& Map :: getSnowMap () const // Get the snow map
{
	assert(isInitialized()); // Ensure the map is initialized

	return snow_map;
}


// Draw blocks of the map
void Map :: drawBlocks () const
{
    // Ensure that the map is initialized
    assert(isInitialized());

    // Loop through each position in the block map
    for(int x = 0; x < block_map.getSizeX(); x++)
        for(int z = 0; z < block_map.getSizeZ(); z++)
        {
            // Get the block count at the current position
            unsigned int count = block_map.getAt(x, z);
            // Draw each block at the current position
            for(unsigned int y = 0; y < count; y++)
                drawBlock(x, y, z); // Function drawBlock is assumed to be defined elsewhere
        }
}

// Draw snow around a given position
void Map :: drawSnow (int center_x, int center_z,
                      int snow_radius) const
{
    // Ensure that the map is initialized
    assert(isInitialized());
    // Ensure that the snow radius is non-negative
    assert(snow_radius >= 0);

    // Draw snow around the specified position with the given radius
    snow_map.drawAround(center_x, center_z, snow_radius);
}


// Initialize the snow map
void Map :: initSnowMap (const string& texture_filename)
{
	noise_large = PerlinNoiseField(2.8f, 0.6f,
	                               0xf574c27b, 0x0ed3ee2f, 0x38638a3b, 0x875f9dc9,
	                               0x5bc9cb6e, 0x729a45e1, 0xe0d7f47e);
	noise_small = PerlinNoiseField(0.43f, 0.3f,
	                               0xc11df3f8, 0xc9c2b70e, 0xa5f71ab8, 0xed536273,
	                               0xd539c909, 0x777cdf43, 0x23da6048);

	snow_map = SnowMap(block_map.getSizeX(), block_map.getSizeZ());

	for(int x = 0; x < snow_map.getSizeX(); x++)
		for(int z = 0; z < snow_map.getSizeZ(); z++)
		{
            // Ensure that the current position is within the block map
			assert(block_map.isInMap(x, z));
            // Create an offset vector for the current position
			Vector3 offset = Vector3(x, block_map.getAt(x, z), z);
            // Initialize the heightmap for snow at the current position
            Heightmap heightmap = initSnowHeightmap(x, z);
            // Initialize the display list for the heightmap with the given offset and texture filename
            heightmap.initDisplayList(offset, texture_filename);
            // Set the display list of the heightmap in the snow map at the current position
            snow_map.setAt(x, z, heightmap.getDisplayList());
		}
}

// Initialize the snow heightmap at a given position
Heightmap Map :: initSnowHeightmap (int base_x, int base_z)
{
	Neighbourhood neighbourhood(block_map, base_x, base_z);

	Heightmap snow;
	for(int vx = 0; vx < HEIGHTMAP_VERTEX_SIZE; vx++)
		for(int vz = 0; vz < HEIGHTMAP_VERTEX_SIZE; vz++)
		{
            // Calculate the fractional position of the vertex within the heightmap
			float x_frac = vx / (HEIGHTMAP_VERTEX_SIZE - 1.0f);
			float z_frac = vz / (HEIGHTMAP_VERTEX_SIZE - 1.0f);
            // Calculate the height value for snow at the current vertex position
			float value = calculateSnowHeight(base_x, base_z, neighbourhood, x_frac, z_frac);
			snow.setAt(vx, vz, value);
		}
	return snow;
}

// Calculate the snow height at a given position
float Map :: calculateSnowHeight (int base_x, int base_z,
                                  const Neighbourhood& neighbourhood,
                                  float x_frac, float z_frac) const
{
    // Calculate large-scale perlin noise value at the given position
    float value_large  = noise_large.perlinNoise(base_x + x_frac, base_z + z_frac);
	float value_small  = noise_small.perlinNoise(base_x + x_frac, base_z + z_frac);
    // Get the distance to the nearest block edge in the positive direction
    float value_higher = (float)(neighbourhood.getEdgeDistance(x_frac, z_frac,  1));
    // Get the distance to the nearest block edge in the negative direction
    float value_lower  = (float)(neighbourhood.getEdgeDistance(x_frac, z_frac, -1));

	float value_noise = value_large + value_small;
    // Adjust the effect of block edges on snow height
    float value_edges = sqrt(value_lower) * 0.4f - sqrt(value_higher) * 0.4f;
	//float value_edges = value_lower * 0.3f - value_higher * 0.3f;
    // Combine noise and edge effects to determine total snow height
    float value_total = value_noise + value_edges;
	if(value_total > value_lower)
		value_total = value_lower;

	return value_total;
}


// Draw a block at a given position
void Map :: drawBlock (int x, int y, int z) const
{
	glPushMatrix();
		glTranslated(x + 0.5, y + 0.5, z + 0.5);
		block_list.draw();
	glPopMatrix();
}

// Check if there is snow at a given position
bool Map :: hasSnowAtPosition(const Vector3& position)
{
    // Convert world position to integer coordinates
    int base_x = static_cast<int>(floor(position.x));
    int base_z = static_cast<int>(floor(position.z));

    // Check if the position is outside the block map
    // Outside the map, no snow
    if (!isInMap(base_x, base_z))
    {
        return false;
    }
    
    float x_frac = position.x - base_x;
    float z_frac = position.z - base_z;

    // Get the block height and snow height at the position
    unsigned int block_height = block_map.getAt(base_x, base_z);
    float snow_height_relative = calculateSnowHeight(base_x, base_z,getNeighbourhood(base_x, base_z),x_frac,z_frac);
    
    // Calculate the sum of heights
    float total_height = static_cast<float>(block_height) + snow_height_relative;

    // Compare with the query position's Y-value
    if(position.y < total_height)
        return true;
    
    return false;
}

// Determine the surface type at a given collision box
surfacetype Map::getPlayerSurfaceType(const Box& collisionBox) {
    // Calculate the "feet" position at the center of the bottom of the box
    Vector3 feetPosition = collisionBox.center;
    feetPosition.y -= collisionBox.half_size.y;

    // Move the test position slightly down to avoid rounding errors
    feetPosition.y -= 0.01;

    // Check if the player is outside the block area
    if (!block_map.isInMap(feetPosition.x,feetPosition.z)) {
        if (feetPosition.y <= 0.0) {
            return GROUND; // Player is on the ground
        } else {
            return AIR; // Player is in the air
        }
    } else {
        if (hasSnowAtPosition(feetPosition)) {
            return SNOW; // Player is on snow
        } else if (feetPosition.y <= 0.0) {
            return GROUND; // Player is on the ground
        }
            else {
                // Find the height of the block stack at the player's position
                double blockStackHeight = getBlockHeight(feetPosition.x,feetPosition.z);
                if (feetPosition.y < blockStackHeight) {
                    return BLOCK; // Player is on a block
                } else {
                    return AIR; // Player is in the air
                }
        }
    }
}
// Check if a node at the given position is considered "high-ish"
bool Map::isHighIshNode(int x, int z) const
{
    // Node is on top of at least one block (not on the ground)
    if (getBlockHeight(x, z) > 0) {
        // None of the block's 8-neighbours are higher
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                if (i == 0 && j == 0) continue;  // Skip the current node

                int neighborX = x + i;
                int neighborZ = z + j;

                // The neighbor is within bounds
                // Check if the neighbor's height is greater than the node's height
                if (neighborX >= 0 && neighborX < getSizeX() && neighborZ >= 0 && neighborZ < getSizeZ()) {
                    if (getBlockHeight(neighborX, neighborZ) > getBlockHeight(x, z)) {
                        return false;  // If one of the neighbors is higher
                    }
                }
            }
        }

        return true;  // Node is high-ish
    }

    return false;  // Node is on the ground
}

// Find snowball nodes for opponents
vector<Vector3> Map :: findSnowballNodes()
{
    std::vector<Vector3> opponentSnowballNodes;
    for (int x = 0; x < getSizeX(); ++x) {
            for (int z = 0; z < getSizeZ(); ++z) {
                Vector3 node_position(x + 0.5f, 0.0f, z + 0.5f);  // 0.5 to find the center

                // Check if the node has at least 0.2 m of snow on it
                if (hasSnowAtPosition(node_position) >= 0.2f) {

                    // Check if the node is not high-ish
                    if (!isHighIshNode(x, z)) {
                        opponentSnowballNodes.push_back(node_position);
                    }
                }
            }
        }
    return opponentSnowballNodes;
}

// Find nodes suitable for throwing snowballs
vector<Vector3> Map::throwSnowballNodes()
{
    std::vector<Vector3> ThrowNodes;
    
    for (int x = 0; x < getSizeX(); ++x) {
        for (int z = 0; z < getSizeZ(); ++z) {
            Vector3 node_position(x + 0.5f, 0.0f, z + 0.5f);  // 0.5 to find the center

            // If the node has at least 0.2 m of snow below the block surface
            if (hasSnowAtPosition(node_position) >= 0.2f) {

                // If the node is high-ish
                if (isHighIshNode(x, z)) {
                    ThrowNodes.push_back(node_position);
                }
            }
        }
    }
    return ThrowNodes;
}

