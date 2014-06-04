//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "BeamSystem.hpp"

namespace Pomdog {
namespace {
//-----------------------------------------------------------------------
static std::vector<Vector2> CreateJaggedLine(
	BeamEmitter const& emitter,
	std::uint32_t interpolationPoints,
	Vector2 const& start, Vector2 const& end, std::mt19937 & random)
{	
	auto tangent = end - start;
	auto normal = Vector2{-tangent.Y, tangent.X};

	std::vector<float> positions;

	for (size_t i = 0; i < interpolationPoints; ++i) {
		std::uniform_real_distribution<float> distribution(0, 1);
		positions.push_back(distribution(random));
	}

	std::sort(std::begin(positions), std::end(positions), std::less<float>());
	

	std::vector<Vector2> points;
	points.reserve(interpolationPoints + 2);
	points.push_back(start);
	
	Vector2 prevPoint = points.front();
	float prevDisplacement = 0;
	float prevPosition = 0;
	
	auto length = std::max(Vector2::Distance(start, end), std::numeric_limits<float>::epsilon());
	POMDOG_ASSERT(length > 0);

	for (auto & position: positions)
	{
		POMDOG_ASSERT(emitter.Jaggedness >= 0);
		float scale = position - prevPosition;
		scale = std::min((scale * interpolationPoints) * emitter.Jaggedness, 1.0f);
		scale = scale * scale;
		POMDOG_ASSERT(scale >= 0);
		
		std::uniform_real_distribution<float> distribution(emitter.SwayRange.a() / length, emitter.SwayRange.b() / length);
		
		POMDOG_ASSERT(!positions.empty());
		
		constexpr float threshold = 0.95f;
		constexpr float envelopeScale = (1.0f / (1.0f - threshold));
		float envelope = (position > threshold) ? envelopeScale * (1.0f - position): 1.0f;
		
		float displacement = MathHelper::SmoothStep(prevDisplacement, distribution(random), scale);
		displacement *= envelope;

		Vector2 point = start + tangent * position + normal * displacement;
		
		prevPoint = point;
		prevDisplacement = displacement;
		prevPosition = position;
		
		points.push_back(std::move(point));
	}

	points.push_back(end);

	POMDOG_ASSERT(!points.empty());
	POMDOG_ASSERT(points.size() >= 2);
	return std::move(points);
}
//-----------------------------------------------------------------------
static std::vector<Vector2> CreateBranch(BeamEmitter const& emitter,
	BeamBranching const& branching,
	Beam const& parentBeam, std::mt19937 & random)
{
	POMDOG_ASSERT(parentBeam.Points.size() >= 2);
	
	std::uniform_int_distribution<std::uint32_t> indexDistribution(
		0, static_cast<std::uint32_t>(parentBeam.Points.size() - 1));
	
	auto index = indexDistribution(random);
	POMDOG_ASSERT(index < parentBeam.Points.size());
	
	Vector2 const& sourceStart = parentBeam.Points.front();
	Vector2 const& sourceEnd = parentBeam.Points.back();
	Vector2 start = parentBeam.Points[index];
	
	Vector2 tangent = sourceEnd - start;
	Vector2 normal {-tangent.Y, tangent.X};

	auto distribution = branching.SpreadRange;
	Vector2 end = sourceEnd + (Vector2::Normalize(normal) * distribution(random));

	auto scale = Vector2::Distance(end, start) / Vector2::Distance(sourceEnd, sourceStart);

	return CreateJaggedLine(emitter, emitter.InterpolationPoints * scale, start, end, random);
}

}// unnamed namespace
//-----------------------------------------------------------------------
BeamSystem::BeamSystem()
	: erapsedTime(0)
	, emissionTimer(0)
	, random(std::random_device{}())
{
	//particles.reserve(emitter.MaxParticles);

	emitter.Looping = true;
	emitter.StartLifetime = 0.64f;
	emitter.SwayRange = std::uniform_real_distribution<float>{-9.0f, 9.0f};
	emitter.InterpolationPoints = 40;
	emitter.Jaggedness = 0.7f;
	emitter.MaxBeams = 100;
	emitter.EmissionRate = 10;
	emitter.StartColor = Color::White;
	emitter.EndColor = {255, 220, 130, 0};
	emitter.StartThickness = 1.2f;
	
	branching.InheritThickness = 0.4f;
	branching.MaxBranches = 4;
	branching.SpreadRange = std::uniform_real_distribution<float>{-40.0f, 40.0f};
	
	//emitter.StartLifetime = 1.0f;
	//emitter.MaxBeams = 1;
}
//-----------------------------------------------------------------------
void BeamSystem::Update(DurationSeconds const& frameDuration, Transform2D const& emitterTransform, Vector2 const& target)
{
	erapsedTime += frameDuration;
	
	if (emitter.Looping && erapsedTime > emitter.Duration)
	{
		erapsedTime = DurationSeconds{0};
	}

	if (emitter.Looping || erapsedTime <= emitter.Duration)
	{
		emissionTimer += frameDuration;
		
		POMDOG_ASSERT(emitter.EmissionRate > 0);
		auto emissionInterval = std::max(std::numeric_limits<DurationSeconds>::epsilon(),
			DurationSeconds(1) / emitter.EmissionRate);
		
		POMDOG_ASSERT(emissionInterval.count() > 0);

		POMDOG_ASSERT(emitter.Duration.count() > 0);
		//float normalizedTime = erapsedTime / emitter.Duration;
		
		auto CreateBranchBeam = [this](Beam const& beam) {
			for (std::size_t i = 0; i < branching.MaxBranches; ++i) {
				if (beams.size() + 1 >= emitter.MaxBeams) {
					break;
				}
				
				Beam branchBeam;
				branchBeam.Points = CreateBranch(emitter, branching, beam, random);
				branchBeam.TimeToLive = beam.TimeToLive;
				branchBeam.Color = beam.Color;
				branchBeam.Thickness = beam.Thickness * branching.InheritThickness;
				beams.push_back(std::move(branchBeam));
			}
		};
		
		while ((beams.size() < emitter.MaxBeams) && (emissionTimer >= emissionInterval))
		{
			emissionTimer -= emissionInterval;

			Beam beam;
			
			POMDOG_ASSERT(emitter.StartLifetime > 0.0f);
			beam.TimeToLive = emitter.StartLifetime;
			beam.Points = CreateJaggedLine(emitter, emitter.InterpolationPoints,
				emitterTransform.Position, target, random);
			beam.Color = emitter.StartColor;
			beam.Thickness = emitter.StartThickness;
			
			CreateBranchBeam(beam);
			beams.push_back(std::move(beam));
		}
	}
	{
		for (auto & beam: beams) {
			beam.TimeToLive -= frameDuration.count();
			if (beam.TimeToLive <= 0.0f) {
				continue;
			}
			
			float normalizedTime = 1.0f - beam.TimeToLive / emitter.StartLifetime;
			
			beam.Color = Color::SmoothStep(emitter.StartColor, emitter.EndColor, normalizedTime);
		}
		
		beams.erase(std::remove_if(std::begin(beams), std::end(beams),
			[](Beam const& beam){ return beam.TimeToLive <= 0; }), std::end(beams));
	}
}
//-----------------------------------------------------------------------
Vector2 BeamSystem::CreateTarget(Transform2D const& emitterTransform, float distance)
{
	Vector2 tangent {
		distance * std::cos(emitterTransform.Rotation.value),
		distance * std::sin(emitterTransform.Rotation.value)};
	auto targetPosition = emitterTransform.Position + tangent;
	return targetPosition;
}
//-----------------------------------------------------------------------
}// namespace Pomdog
