#version 330 core
in vec2 vUV;
out vec4 fragColor;

uniform vec2 uPlayerPos;
uniform float uPlayerAngle;
uniform float uFov;
uniform vec2 uMapSize;
uniform float uHasKey;
uniform vec2 uResolution;

uniform sampler2D uMapTex;

const float MAX_DEPTH = 20.0;
const float FOG_DIST = 12.0;

// Cell types
const float C_EMPTY = 0.0;
const float C_WALL  = 1.0;
const float C_DOOR  = 2.0;
const float C_KEY   = 3.0;
const float C_EXIT  = 4.0;

float sampleMap(vec2 p) {
    vec2 uv = vec2((p.x + 0.5) / uMapSize.x, 1.0 - (p.y + 0.5) / uMapSize.y);
    return texture(uMapTex, uv).r * 255.0;
}

void main() {
    float aspect = uResolution.x / uResolution.y;
    float halfFov = uFov * 0.5;
    float rayAngle = uPlayerAngle - halfFov + (vUV.x * uFov);
    vec2 dir = vec2(cos(rayAngle), sin(rayAngle));

    // DDA raymarch
    float dist = 0.0;
    float cellType = C_EMPTY;
    vec2 hitCell = vec2(-1.0);
    float stepSize = 0.04;

    for (int i = 0; i < 400; i++) {
        dist += stepSize;
        vec2 pos = uPlayerPos + dir * dist;
        vec2 cell = floor(pos);
        cellType = sampleMap(cell);

        if (cellType >= C_WALL) {
            hitCell = cell;
            break;
        }
    }

    // Sky (ceiling) and floor by screen y
    if (cellType < C_WALL) {
        vec3 sky = vec3(0.15, 0.2, 0.35);
        vec3 floorCol = vec3(0.12, 0.1, 0.08);
        vec3 col = mix(floorCol, sky, vUV.y);
        float fog = 1.0 - exp(-dist / FOG_DIST);
        col = mix(col, vec3(0.4, 0.45, 0.5), fog * 0.6);
        fragColor = vec4(col, 1.0);
        return;
    }

    // Wall color by cell type
    vec3 wallCol;
    if (cellType >= C_EXIT - 0.5)
        wallCol = vec3(0.2, 0.6, 0.25);
    else if (cellType >= C_KEY - 0.5)
        wallCol = vec3(0.85, 0.7, 0.2);
    else if (cellType >= C_DOOR - 0.5)
        wallCol = uHasKey > 0.5 ? vec3(0.25, 0.2, 0.15) : vec3(0.35, 0.25, 0.15);
    else
        wallCol = vec3(0.4, 0.35, 0.3);

    float shade = 1.0 - (dist / MAX_DEPTH) * 0.5;
    wallCol *= shade;

    float fog = 1.0 - exp(-dist / FOG_DIST);
    wallCol = mix(wallCol, vec3(0.35, 0.38, 0.4), fog);

    fragColor = vec4(wallCol, 1.0);
}
