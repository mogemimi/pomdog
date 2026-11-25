// Copyright mogemimi. Distributed under the MIT license.

// NOTE: This shader is Pomdog's own anti-aliasing filter based on the ideas of
// FXAA. Like the quality variant of FXAA 3.11 by Timothy Lottes, it finds the
// orientation of an edge in the 3x3 neighborhood, searches along the edge for
// the ends of its stair step, and moves the final fetch across the edge by how
// close the pixel is to the nearer end. It differs from FXAA 3.11 as follows:
//
// - It finds lines one pixel wide, which FXAA 3.11 treats as edges and fades.
//   For a pixel on such a line and for a pixel beside it, it searches along
//   the line and blends toward the row that the line steps into, so the line
//   keeps its brightness.
// - It searches with whole-pixel steps, takes the middle between the last
//   fetch on the edge and the first fetch past it as the end, and averages the
//   blend over the half pixel that the end is uncertain by. This also blends
//   diagonal edges, which FXAA 3.11 leaves as they are.
// - It does not blend toward an end that is a corner of a shape, so the
//   corners of rectangles stay sharp and pixel-aligned shapes stay unchanged.
// - It skips pixels whose luma lies well inside the local range, which are
//   already anti-aliased, such as text and filtered sprites.
// - It has no subpixel term, which blurs pixel-aligned lines and textures.

struct ImageEffectConstants {
    float2 RenderTargetPixelSize;
    float2 RcpFrame;
};

struct FxaaBlock {
    // Local contrast that a pixel needs, relative to its brightest neighbor,
    // to be treated as an edge.
    float EdgeThreshold;

    // Local contrast that a pixel needs to be treated as an edge, which skips
    // dark areas.
    float EdgeThresholdMin;
};

[[vk::binding(0, 0)]]
ConstantBuffer<ImageEffectConstants> imageEffectCb;

[[vk::binding(1, 0)]]
ConstantBuffer<FxaaBlock> cb;

[[vk::binding(0, 1)]]
Sampler2D DiffuseTexture;

// NOTE: Step lengths in pixels of the search for the ends of an edge, after
// the first step of one pixel, which the 3x3 neighborhood already covers.
// Whole-pixel steps keep every fetch on a texel center along the edge, so a
// fetch never mixes the texels before and after an end.
static const int FxaaSearchStepCount = 4;
static const float FxaaSearchSteps[FxaaSearchStepCount] = {1.0, 1.0, 2.0, 4.0};

// NOTE: Distance in pixels that the search assumes is left to the end of an
// edge after it runs out of steps.
static const float FxaaSearchExtrapolation = 8.0;

// NOTE: A pixel whose luma is farther than this fraction of the local range
// from both the minimum and the maximum is treated as already anti-aliased.
static const float FxaaMidtoneSkip = 0.1;

// NOTE: The smaller of the two gradients across a ridge must be at least this
// fraction of the larger one for the ridge to count as a thin line.
static const float FxaaRidgeRatio = 0.5;

// NOTE: The pixel is on an edge between two areas.
static const int FxaaModeEdge = 0;

// NOTE: The pixel is on a line one pixel wide.
static const int FxaaModeLine = 1;

// NOTE: The pixel is next to a line one pixel wide.
static const int FxaaModeBesideLine = 2;

float FxaaLuma(float3 rgb)
{
    // NOTE: FXAA expects perceptual luma, so this weights the gamma-encoded
    // colors without converting them to linear space.
    return dot(rgb, float3(0.299, 0.587, 0.114));
}

float FxaaSampleLuma(Sampler2D tex, float2 pos)
{
    return FxaaLuma(tex.SampleLevel(pos, 0.0).rgb);
}

float FxaaSampleLumaOffset(Sampler2D tex, float2 pos, constexpr int2 offset)
{
    return FxaaLuma(tex.SampleLevel(pos, 0.0, offset).rgb);
}

// NOTE: Returns whether a pixel with these gradients to its two neighbors
// across an edge is brighter or darker than both by similar amounts, which
// means that the pixel is on a line one pixel wide.
bool FxaaIsRidge(float gradientA, float gradientB)
{
    return (gradientA * gradientB > 0.0) &&
        (min(abs(gradientA), abs(gradientB)) >= FxaaRidgeRatio * max(abs(gradientA), abs(gradientB)));
}

// NOTE: Returns how far to blend a pixel across a straight edge at distance b
// from the end of a run of length L. The blend is 0.5 - b / L, and this
// averages it over [b - 0.5, b + 0.5] because the end is known only to within
// a pixel. For runs of two pixels or more, the result equals 0.5 - b / L at
// the last pixel of the run, and for diagonal runs of one pixel it is 1/8.
float FxaaRunBlend(float b, float runLength)
{
    float lo = max(b - 0.5, 0.0);
    float hi = min(b + 0.5, runLength * 0.5);
    if (hi <= lo) {
        return 0.0;
    }
    return 0.5 * (hi - lo) - (hi * hi - lo * lo) / (2.0 * runLength);
}

float3 FxaaPixelShader(
    Sampler2D tex,
    float2 posM,
    float2 rcpFrame,
    float edgeThreshold,
    float edgeThresholdMin)
{
    // NOTE:
    //     NW N NE
    //     W  M  E
    //     SW S SE

    float3 rgbM = tex.SampleLevel(posM, 0.0).rgb;
    float lumaM = FxaaLuma(rgbM);

    // NOTE: Test the contrast coarsely first, with two bilinear fetches at the
    // texel corners, each the average of a 2x2 block that holds the center and
    // two neighbors of the cross. Most pixels in a frame end here after three
    // fetches. One texel that differs moves the average of its block by a
    // quarter of the difference, so the test multiplies the contrast by four
    // to keep the pixels that pass the exact test below. Brighter and darker
    // neighbors in one block can cancel out, but then the center lies between
    // them, and the midtone test below would skip the pixel anyway.
    {
        float lumaBlockNW = FxaaSampleLuma(tex, posM - rcpFrame * 0.5);
        float lumaBlockSE = FxaaSampleLuma(tex, posM + rcpFrame * 0.5);
        float blockContrast = max(abs(lumaBlockNW - lumaM), abs(lumaBlockSE - lumaM)) * 4.0;
        float lumaBlockMax = max(max(lumaBlockNW, lumaBlockSE), lumaM);
        if (blockContrast < max(edgeThresholdMin, lumaBlockMax * edgeThreshold)) {
            return rgbM;
        }
    }

    float lumaN = FxaaSampleLumaOffset(tex, posM, int2(0, -1));
    float lumaW = FxaaSampleLumaOffset(tex, posM, int2(-1, 0));
    float lumaE = FxaaSampleLumaOffset(tex, posM, int2(1, 0));
    float lumaS = FxaaSampleLumaOffset(tex, posM, int2(0, 1));

    float lumaMax = max(max(max(lumaN, lumaW), max(lumaE, lumaS)), lumaM);
    float lumaMin = min(min(min(lumaN, lumaW), min(lumaE, lumaS)), lumaM);
    float lumaRange = lumaMax - lumaMin;

    // NOTE: Skip pixels whose local contrast is too low to be an aliased edge.
    if (lumaRange < max(edgeThresholdMin, lumaMax * edgeThreshold)) {
        return rgbM;
    }

    // NOTE: An aliased edge has only the colors of its two sides, so a center
    // luma well inside the local range means the pixel is already a blend of
    // its neighbors, such as an anti-aliased edge or a gradient. Filtering it
    // again would only blur it.
    float midtone = (lumaM - lumaMin) / lumaRange;
    if (min(midtone, 1.0 - midtone) > FxaaMidtoneSkip) {
        return rgbM;
    }

    float lumaNW = FxaaSampleLumaOffset(tex, posM, int2(-1, -1));
    float lumaNE = FxaaSampleLumaOffset(tex, posM, int2(1, -1));
    float lumaSW = FxaaSampleLumaOffset(tex, posM, int2(-1, 1));
    float lumaSE = FxaaSampleLumaOffset(tex, posM, int2(1, 1));

    // NOTE: Compare the second derivatives across rows and across columns to
    // tell whether the edge runs horizontally or vertically.
    float edgeHorizontal =
        abs(lumaNW + lumaSW - 2.0 * lumaW) +
        abs(lumaN + lumaS - 2.0 * lumaM) * 2.0 +
        abs(lumaNE + lumaSE - 2.0 * lumaE);
    float edgeVertical =
        abs(lumaNW + lumaNE - 2.0 * lumaN) +
        abs(lumaW + lumaE - 2.0 * lumaM) * 2.0 +
        abs(lumaSW + lumaSE - 2.0 * lumaS);
    bool isHorizontal = edgeHorizontal >= edgeVertical;

    // NOTE: perp is one pixel across the edge, toward the positive neighbor,
    // and along is one pixel along the edge.
    float2 perp = isHorizontal ? float2(0.0, rcpFrame.y) : float2(rcpFrame.x, 0.0);
    float2 along = isHorizontal ? float2(rcpFrame.x, 0.0) : float2(0.0, rcpFrame.y);
    float lumaNeg = isHorizontal ? lumaN : lumaW;
    float lumaPos = isHorizontal ? lumaS : lumaE;
    float gradientNeg = lumaNeg - lumaM;
    float gradientPos = lumaPos - lumaM;

    // NOTE: The pair is the neighbor across the edge with the steeper gradient.
    // The search runs along the row between the center and the pair, and an
    // end is where the luma there leaves the average of the two.
    int mode = FxaaModeEdge;
    float pairSign = (abs(gradientNeg) >= abs(gradientPos)) ? -1.0 : 1.0;
    float lumaPair = (pairSign < 0.0) ? lumaNeg : lumaPos;
    float2 posStart = posM + perp * (pairSign * 0.5);
    float lumaRef = (lumaPair + lumaM) * 0.5;
    float threshold = abs(lumaPair - lumaM) * 0.25;

    if (FxaaIsRidge(gradientNeg, gradientPos)) {
        // NOTE: The center is on a thin line, so search along the line itself.
        mode = FxaaModeLine;
        posStart = posM;
        lumaRef = lumaM;
        threshold = abs(lumaM - (lumaNeg + lumaPos) * 0.5) * 0.5;
    }
    else {
        // NOTE: Check whether the pair is on a thin line, with the pixel past it.
        float lumaFar = FxaaSampleLuma(tex, posM + perp * (pairSign * 2.0));
        if (FxaaIsRidge(lumaM - lumaPair, lumaFar - lumaPair)) {
            mode = FxaaModeBesideLine;
            posStart = posM + perp * pairSign;
            lumaRef = lumaPair;
            threshold = abs(lumaPair - (lumaM + lumaFar) * 0.5) * 0.5;
        }
    }

    // NOTE: The first step is one pixel away, where the 3x3 neighborhood
    // already has the texels. Bilinear filtering and luma are both linear, so
    // the average of two lumas equals the luma of a fetch between them.
    float lumaAlongNegM = isHorizontal ? lumaW : lumaN;
    float lumaAlongPosM = isHorizontal ? lumaE : lumaS;
    float lumaAlongNegPair = (pairSign < 0.0) ? lumaNW : (isHorizontal ? lumaSW : lumaNE);
    float lumaAlongPosPair = (pairSign < 0.0) ? (isHorizontal ? lumaNE : lumaSW) : lumaSE;
    float lumaFirstN = lumaAlongNegPair;
    float lumaFirstP = lumaAlongPosPair;
    if (mode == FxaaModeEdge) {
        lumaFirstN = (lumaAlongNegM + lumaAlongNegPair) * 0.5;
        lumaFirstP = (lumaAlongPosM + lumaAlongPosPair) * 0.5;
    }
    else if (mode == FxaaModeLine) {
        lumaFirstN = lumaAlongNegM;
        lumaFirstP = lumaAlongPosM;
    }

    // NOTE: Search in both directions. insideN and insideP are the distances of
    // the last fetches that were still on the edge.
    float distN = 1.0;
    float distP = 1.0;
    float insideN = 0.0;
    float insideP = 0.0;
    float lumaEndN = lumaFirstN - lumaRef;
    float lumaEndP = lumaFirstP - lumaRef;
    bool doneN = abs(lumaEndN) >= threshold;
    bool doneP = abs(lumaEndP) >= threshold;

    // NOTE: Fetch every step in both directions first, and then walk them.
    // The fetches do not depend on each other, so the GPU overlaps them. This
    // is faster than waiting for each fetch to decide on the next one, even
    // though a pixel near an end fetches texels that it does not use.
    float lumaStepN[FxaaSearchStepCount];
    float lumaStepP[FxaaSearchStepCount];
    float stepDistance = 1.0;
    [ForceUnroll]
    for (int i = 0; i < FxaaSearchStepCount; ++i) {
        stepDistance += FxaaSearchSteps[i];
        lumaStepN[i] = FxaaSampleLuma(tex, posStart - along * stepDistance) - lumaRef;
        lumaStepP[i] = FxaaSampleLuma(tex, posStart + along * stepDistance) - lumaRef;
    }

    // NOTE: The loop has no break because a break in an unrolled loop turns
    // into deeply nested blocks in the transpiled GLSL and MSL.
    [ForceUnroll]
    for (int i = 0; i < FxaaSearchStepCount; ++i) {
        if (!doneN) {
            insideN = distN;
            distN += FxaaSearchSteps[i];
            lumaEndN = lumaStepN[i];
            doneN = abs(lumaEndN) >= threshold;
        }
        if (!doneP) {
            insideP = distP;
            distP += FxaaSearchSteps[i];
            lumaEndP = lumaStepP[i];
            doneP = abs(lumaEndP) >= threshold;
        }
    }
    if (!doneN) {
        insideN = distN;
        distN += FxaaSearchExtrapolation;
    }
    if (!doneP) {
        insideP = distP;
        distP += FxaaSearchExtrapolation;
    }

    // NOTE: An end lies between the last fetch on the edge and the first fetch
    // past it, so take the middle of the two.
    float boundN = (insideN + distN) * 0.5;
    float boundP = (insideP + distP) * 0.5;
    float blend = FxaaRunBlend(min(boundN, boundP), boundN + boundP);

    // NOTE: Pick the nearer end without more fetches. On a tie, an edge takes
    // the end where the center's side ends, a pixel beside a line takes the
    // end where its neighbor along the row shows the line going on, and a
    // pixel on a line takes the N end.
    bool isLumaMSmaller = lumaM < lumaRef;
    bool isGoodN = (lumaEndN < 0.0) != isLumaMSmaller;
    float lumaAlongNegRow = (mode == FxaaModeBesideLine) ? lumaAlongNegM : lumaRef;
    float lumaAlongPosRow = (mode == FxaaModeBesideLine) ? lumaAlongPosM : lumaRef;
    bool tieN = true;
    if (mode == FxaaModeEdge) {
        tieN = isGoodN;
    }
    else if (mode == FxaaModeBesideLine) {
        tieN = abs(lumaAlongNegRow - lumaRef) <= abs(lumaAlongPosRow - lumaRef);
    }
    bool useN = (abs(boundN - boundP) < 0.25) ? tieN : (boundN < boundP);
    float2 posEnd = posStart + along * (useN ? -distN : distP);
    float lumaEnd = useN ? lumaEndN : lumaEndP;

    // NOTE: Read the same two texels past the end in every mode, so that
    // neighboring pixels in different modes do not branch into fetches of
    // their own. They are the rows on both sides of the searched row.
    float side = (mode == FxaaModeLine) ? 1.0 : pairSign;
    bool goesOnA = abs(FxaaSampleLuma(tex, posEnd - perp * side) - lumaRef) < threshold;
    bool goesOnB = abs(FxaaSampleLuma(tex, posEnd + perp * side) - lumaRef) < threshold;

    float direction = 0.0;
    if (mode == FxaaModeEdge) {
        // NOTE: Blend only when the end is where the center's side ends, and
        // the edge goes on past it as the next stair step, one pixel further
        // from the pair. Otherwise the end is a corner of a shape, such as a
        // rectangle, and blending toward it would round the corner.
        bool isGood = (lumaEnd < 0.0) != isLumaMSmaller;
        if (isGood && goesOnA) {
            direction = pairSign;
        }
    }
    else if (mode == FxaaModeLine) {
        // NOTE: Blend toward the row that the line steps into past its end.
        if (goesOnA != goesOnB) {
            direction = goesOnA ? -1.0 : 1.0;
        }
    }
    else {
        // NOTE: Blend toward the line only where it steps into this row.
        if (goesOnA && !goesOnB) {
            direction = pairSign;
        }
    }

    if (direction == 0.0) {
        return rgbM;
    }
    return tex.SampleLevel(posM + perp * (direction * blend), 0.0).rgb;
}

[shader("pixel")]
float4 fxaa_ps(
    [[vk::location(0)]] float2 TextureCoord : TEXCOORD0
) : SV_Target0
{
    float3 color = FxaaPixelShader(
        DiffuseTexture,
        TextureCoord,
        imageEffectCb.RcpFrame,
        cb.EdgeThreshold,
        cb.EdgeThresholdMin);
    return float4(color, 1.0);
}
