// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_IMPELLER_ENTITY_ENTITY_H_
#define FLUTTER_IMPELLER_ENTITY_ENTITY_H_

#include <cstdint>

#include "impeller/core/capture.h"
#include "impeller/entity/contents/contents.h"
#include "impeller/geometry/color.h"
#include "impeller/geometry/matrix.h"
#include "impeller/geometry/rect.h"

namespace impeller {

class Renderer;
class RenderPass;

class Entity {
 public:
  static constexpr BlendMode kLastPipelineBlendMode = BlendMode::kModulate;
  static constexpr BlendMode kLastAdvancedBlendMode = BlendMode::kLuminosity;

  enum class RenderingMode {
    /// In direct mode, the Entity's transform is used as the current
    /// local-to-screen transform matrix.
    kDirect,
    /// In subpass mode, the Entity passed through the filter is in screen space
    /// rather than local space, and so some filters (namely,
    /// MatrixFilterContents) need to interpret the given EffectTransform as the
    /// current transform matrix.
    kSubpass,
  };

  /// An enum to define how to repeat, fold, or omit colors outside of the
  /// typically defined range of the source of the colors (such as the
  /// bounds of an image or the defining geometry of a gradient).
  enum class TileMode {
    /// Replicate the edge color if the shader draws outside of its original
    /// bounds.
    kClamp,

    /// Repeat the shader's image horizontally and vertically (or both along and
    /// perpendicular to a gradient's geometry).
    kRepeat,

    /// Repeat the shader's image horizontally and vertically, seamlessly
    /// alternating mirrored images.
    kMirror,

    /// Render the shader's image pixels only within its original bounds. If the
    /// shader draws outside of its original bounds, transparent black is drawn
    /// instead.
    kDecal,
  };

  enum class ClipOperation {
    kDifference,
    kIntersect,
  };

  /// @brief  Create an entity that can be used to render a given snapshot.
  static std::optional<Entity> FromSnapshot(
      const std::optional<Snapshot>& snapshot,
      BlendMode blend_mode = BlendMode::kSourceOver,
      uint32_t clip_depth = 0);

  Entity();

  ~Entity();

  Entity(Entity&&);

  /// @brief  Get the global transform matrix for this Entity.
  const Matrix& GetTransform() const;

  /// @brief  Set the global transform matrix for this Entity.
  void SetTransform(const Matrix& transform);

  std::optional<Rect> GetCoverage() const;

  Contents::ClipCoverage GetClipCoverage(
      const std::optional<Rect>& current_clip_coverage) const;

  bool ShouldRender(const std::optional<Rect>& clip_coverage) const;

  void SetContents(std::shared_ptr<Contents> contents);

  const std::shared_ptr<Contents>& GetContents() const;

  void SetClipDepth(uint32_t clip_depth);

  void IncrementStencilDepth(uint32_t increment);

  uint32_t GetClipDepth() const;

  void SetNewClipDepth(uint32_t clip_depth);

  uint32_t GetNewClipDepth() const;

  float GetShaderClipDepth() const;

  void SetBlendMode(BlendMode blend_mode);

  BlendMode GetBlendMode() const;

  bool Render(const ContentContext& renderer, RenderPass& parent_pass) const;

  static bool IsBlendModeDestructive(BlendMode blend_mode);

  bool CanInheritOpacity() const;

  bool SetInheritedOpacity(Scalar alpha);

  std::optional<Color> AsBackgroundColor(ISize target_size) const;

  Scalar DeriveTextScale() const;

  Capture& GetCapture() const;

  void SetCapture(Capture capture) const;

  Entity Clone() const;

 private:
  Entity(const Entity&);

  Matrix transform_;
  std::shared_ptr<Contents> contents_;
  BlendMode blend_mode_ = BlendMode::kSourceOver;
  uint32_t clip_depth_ = 0u;
  uint32_t new_clip_depth_ = 1u;
  mutable Capture capture_;
};

}  // namespace impeller

#endif  // FLUTTER_IMPELLER_ENTITY_ENTITY_H_
