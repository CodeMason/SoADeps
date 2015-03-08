/// 
///  IRenderPipeline.h
///  Vorb Engine
///
///  Created by Ben Arnold on 28 Oct 2014
///  Copyright 2014 Regrowth Studios
///  All Rights Reserved
///  
///  Summary:
///  This file provides an interface for a render pipeline.
///

#pragma once

#ifndef IRenderPipeline_h__
#define IRenderPipeline_h__

namespace vorb {
    namespace graphics {

        class IRenderStage;

        /// This abstract class is an interface for a rendering pipeline
        class IRenderPipeline {
        public:
            IRenderPipeline();
            virtual ~IRenderPipeline();

            /// Renders the pipeline
            virtual void render() = 0;

            /// Frees all resources
            virtual void destroy() = 0;
        };
    }
}
namespace vg = vorb::graphics;

#endif // IRenderPipeline_h__