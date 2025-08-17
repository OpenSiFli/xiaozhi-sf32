---
home: true
icon: house
title: 主页
heroImage: /assets/image/xiaozhi-logo.svg
bgImage: https://theme-hope-assets.vuejs.press/bg/6-light.svg
bgImageDark: https://theme-hope-assets.vuejs.press/bg/6-dark.svg
bgImageStyle:
  background-attachment: fixed
heroText: 小智百科全书
tagline: 基于SF32平台 -- 构建全新语音交互体验
heroImageDark: /assets/image/xiaozhi-logo.svg
heroAlt: 小智AI Logo - 智能语音平台
actions:
  - text: 快速入门
    icon: lightbulb
    link: ./get-started/
    type: primary
  - text: 架构设计
    icon: sitemap
    link: ./architecture/
    type: default
  - text: SDK文档
    icon: book
    link: https://docs.sifli.com/projects/sdk/latest/sf32lb52x/index.html
    type: default
  - text: 社区论坛
    icon: comments
    link: https://bbs.sifli.com/
    type: default

highlights:
  - header: SF32芯片核心
    description: 基于先进的SF32芯片平台，为小智提供强大的图形能力和低功耗优势。采用ARM Cortex-M33内核，集成丰富外设，满足多种应用场景。
    bgImage: https://www.sifli.com/sites/default/files/2024-06/frontslider.jpg
    bgImageDark: https://www.sifli.com/sites/default/files/2024-06/frontslider.jpg
    bgImageStyle:
      background-size: cover
      background-position: center
      background-repeat: no-repeat
      background-attachment: fixed
      min-height: 600px
    highlights:
      - title: 高性能处理器
        icon: microchip
        details: ARM Cortex-M33内核，集成高性能2D/2.5D图形引擎，人工智能神经网络加速器，双模蓝牙5.3，以及音频CODEC，提供卓越的超低功耗人工智能物联网（AIoT）场景体验。

      - title: 丰富外设接口
        icon: plug
        details: 集成UART、SPI、I2C、ADC、PWM等多种外设，GPIO资源丰富，满足各种应用场景。

      - title: 超低功耗设计
        icon: battery-half
        details: 多种低功耗模式，CoreMark 功耗效率：低至23uA/MHz @3.8V，内置高效率Buck及低功耗LDO，休眠功耗仅2uA，延长设备续航时间。

      - title: 硬件级安全
        icon: shield-halved
        details: 集成AES 加速器、HASH 加速器、CRC加速器等硬件加密引擎，保护用户数据安全。

      - title: 完善工具链
        icon: screwdriver-wrench
        details: 提供完整的SDK和开发工具，支持多种IDE环境，快速落地你的任何灵感。

  - header: ⚡ 易于开发部署
    description: 提供完整的开发框架和工具链，从硬件设计到软件开发，从本地调试到云端部署，全方位支持开发者快速构建智能语音应用。
    image: /assets/image/development.svg
    bgImage: https://theme-hope-assets.vuejs.press/bg/9-light.svg
    bgImageDark: https://theme-hope-assets.vuejs.press/bg/9-dark.svg
    highlights:
      - title: 模块化架构
        icon: cubes
        details: 采用分层模块化设计，支持灵活配置和自定义扩展

      - title: 丰富的SDK工具包
        icon: code
        details: 提供完整的SDK和API，详细的接口文档，简化开发流程

      - title: 海量例程代码
        icon: file-code
        details: 包含100+示例代码和完整应用案例，覆盖各种使用场景，快速上手开发

      - title: 云端服务集成
        icon: cloud
        details: 支持与云端服务主流平台集成，实现端云协同的智能服务

      - title: OTA无线升级
        icon: download
        details: 支持固件和AI模型的空中升级，远程更新设备功能，降低维护成本

      - title: 专业调试工具
        icon: bug
        details: 提供性能分析、日志监控、错误诊断等调试工具，提升开发效率

  - header: 🌟 开源生态社区
    description: 完全开源的解决方案，拥有活跃的开发者社区和持续的技术创新。我们致力于构建开放、协作、共赢的智能语音生态系统。
    image: /assets/image/community.svg
    bgImage: https://theme-hope-assets.vuejs.press/bg/1-light.svg
    bgImageDark: https://theme-hope-assets.vuejs.press/bg/1-dark.svg
    highlights:
      - title: MIT开源许可
        icon: certificate
        details: 基于MIT许可证完全开源，支持商业和非商业应用，无版权限制

      - title: 活跃开发社区
        icon: users
        details: 完善的开发者社区，官方24小时技术支持，快速响应问题和需求

      - title: 持续版本更新
        icon: arrows-rotate
        details: 定期发布新版本，持续优化性能和增加新功能，保持技术领先

      - title: 丰富技术资源
        icon: blog
        details: 详细的技术文档、视频教程、最佳实践分享，助力开发者快速成长

      - title: 开放贡献机制
        icon: handshake
        details: 欢迎全球开发者贡献代码，提供完善的贡献指南

copyright: false
footer: MIT Licensed | Copyright © 2025 思澈科技（南京）有限公司
---

<style>
  .vp-hero, .hero, .vp-highlight { display: none !important; }
</style>

<script>
  if (typeof document !== 'undefined') {
    document.addEventListener('DOMContentLoaded', function() {
      const hideElements = document.querySelectorAll('.vp-hero, .hero, .vp-highlight, .vp-features');
      hideElements.forEach(el => {
        el.style.display = 'none';
        el.style.visibility = 'hidden';
        el.style.position = 'absolute';
        el.style.left = '-99999px';
      });
      
      const isDark = document.documentElement.classList.contains('dark');
      document.body.style.background = isDark ? '#0d1117' : '#ffffff';
      document.body.style.overflow = 'hidden';
    });
  }
</script>

<div id="fullpage-app"></div>

<style>
#fullpage-wrapper {
  position: fixed !important;
  top: 0 !important;
  left: 0 !important;
  width: 100vw !important;
  height: 100vh !important;
  overflow: hidden !important;
  z-index: 999 !important;
  background: white !important;
}

.dark #fullpage-wrapper {
  background: #0d1117 !important;
}

#app, .vp-app, .theme-container, .vp-page, .vp-article-wrapper {
  overflow: hidden !important;
  background: transparent !important;
}

.sidebar, .vp-sidebar, aside {
  display: none !important;
}

#fullpage-wrapper .fp-section {
  position: relative;
  width: 100%;
  height: 100vh;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  overflow: hidden;
  background: #ffffff;
}

.dark #fullpage-wrapper .fp-section {
  background: #0d1117;
}

#fullpage-wrapper .fp-section-content {
  max-width: 1400px;
  width: 100%;
  padding: 60px 40px 40px 40px;
  box-sizing: border-box;
}

#fullpage-wrapper .fp-container {
  transition: transform 1s cubic-bezier(0.645, 0.045, 0.355, 1);
  will-change: transform;
}

#fullpage-wrapper .fp-hero {
  background: linear-gradient(135deg, rgba(102, 126, 234, 0.1) 0%, rgba(118, 75, 162, 0.1) 100%), #ffffff;
  position: relative;
}

.dark #fullpage-wrapper .fp-hero {
  background: linear-gradient(135deg, rgba(102, 126, 234, 0.1) 0%, rgba(118, 75, 162, 0.1) 100%), #0d1117;
}

.navbar {
  position: fixed !important;
  top: 0;
  left: 0;
  width: 100%;
  z-index: 9999 !important;
  background: rgba(255, 255, 255, 0.98) !important;
  backdrop-filter: blur(20px) !important;
  border-bottom: 1px solid rgba(0, 0, 0, 0.05);
  box-shadow: 0 2px 12px rgba(0, 0, 0, 0.08);
}

.dark .navbar {
  background: rgba(13, 17, 23, 0.98) !important;
  border-bottom-color: rgba(255, 255, 255, 0.05);
}

#fp-nav {
  position: fixed;
  right: 30px;
  top: 50%;
  transform: translateY(-50%);
  z-index: 9998 !important;
  display: flex;
  flex-direction: column;
  gap: 16px;
}

#fp-nav .fp-nav-item {
  width: 12px;
  height: 12px;
  border-radius: 50%;
  background: rgba(102, 126, 234, 0.3);
  cursor: pointer;
  transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
  position: relative;
}

#fp-nav .fp-nav-item:hover {
  background: rgba(102, 126, 234, 0.5);
  transform: scale(1.3);
}

#fp-nav .fp-nav-item.active {
  background: linear-gradient(45deg, #667eea, #764ba2);
  box-shadow: 0 0 20px rgba(102, 126, 234, 0.6);
  transform: scale(1.2);
}

#fp-nav .fp-nav-item::after {
  content: attr(data-tooltip);
  position: absolute;
  right: 24px;
  top: 50%;
  transform: translateY(-50%);
  background: rgba(0, 0, 0, 0.6);
  color: white;
  padding: 6px 12px;
  border-radius: 6px;
  white-space: nowrap;
  font-size: 13px;
  opacity: 0;
  pointer-events: none;
  transition: opacity 0.3s ease;
}

#fp-nav .fp-nav-item:hover::after {
  opacity: 1;
}

.fp-scroll-hint {
  position: absolute;
  bottom: 40px;
  left: 50%;
  transform: translateX(-50%);
  z-index: 100;
  cursor: pointer;
  animation: fp-fade-in 1s ease-out 1.2s both, fp-bounce 2s infinite 2.2s;
}

.fp-scroll-hint::before {
  content: '';
  display: block;
  width: 26px;
  height: 40px;
  border: 2px solid rgba(102, 126, 234, 0.5);
  border-radius: 13px;
  position: relative;
}

.fp-scroll-hint::after {
  content: '';
  display: block;
  width: 4px;
  height: 10px;
  background: rgba(102, 126, 234, 0.6);
  border-radius: 2px;
  position: absolute;
  top: 10px;
  left: 50%;
  transform: translateX(-50%);
  animation: fp-scroll-dot 2s infinite;
}

@keyframes fp-bounce {
  0%, 20%, 50%, 80%, 100% { transform: translateX(-50%) translateY(0); }
  40% { transform: translateX(-50%) translateY(-12px); }
  60% { transform: translateX(-50%) translateY(-6px); }
}

@keyframes fp-scroll-dot {
  0% { opacity: 1; top: 10px; }
  100% { opacity: 0; top: 20px; }
}

.fp-hero-content {
  text-align: center;
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 2rem;
}

.fp-hero-image {
  position: relative;
  margin: 2rem 0;
  width: 280px;
  height: 280px;
  animation: fp-fade-in 1.5s ease-out 0.2s both;
  display: flex;
  align-items: center;
  justify-content: center;
}

.fp-hero-image::before {
  content: '';
  position: absolute;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  width: 320px;
  height: 320px;
  border-radius: 50%;
  background: radial-gradient(circle, rgba(102, 126, 234, 0.1) 0%, transparent 70%);
  animation: fp-glow-pulse 3s ease-in-out infinite;
  z-index: -1;
}

.fp-hero-image {
  width: 280px !important;
  height: 280px !important;
  border-radius: 50% !important;
  background: linear-gradient(145deg, rgba(102, 126, 234, 0.1) 0%, rgba(118, 75, 162, 0.1) 100%) !important;
  border: 2px solid rgba(102, 126, 234, 0.2) !important;
  box-shadow: 
    0 20px 40px rgba(102, 126, 234, 0.15),
    0 0 0 8px rgba(255, 255, 255, 0.1),
    0 0 0 16px rgba(102, 126, 234, 0.05) !important;
  animation: fp-fade-in 1.5s ease-out 0.2s both,
             fp-gentle-float 6s ease-in-out 2s infinite !important;
  position: relative;
  z-index: 1;
  transition: box-shadow 0.3s ease !important;
  display: flex !important;
  align-items: center !important;
  justify-content: center !important;
  padding: 40px !important;
  box-sizing: border-box !important;
}

.fp-hero-image img {
  width: 100% !important;
  height: 100% !important;
  border-radius: 0 !important;
  object-fit: contain !important;
  background: transparent !important;
  border: none !important;
  box-shadow: none !important;
  animation: none !important;
  position: relative;
  z-index: 2;
  max-width: 200px !important;
  max-height: 200px !important;
}

.dark .fp-hero-image {
  box-shadow: 
    0 20px 40px rgba(102, 126, 234, 0.25),
    0 0 0 8px rgba(255, 255, 255, 0.05),
    0 0 0 16px rgba(102, 126, 234, 0.1) !important;
  border-color: rgba(102, 126, 234, 0.3) !important;
}

.fp-hero-image:hover {
  box-shadow: 
    0 25px 50px rgba(102, 126, 234, 0.25),
    0 0 0 8px rgba(255, 255, 255, 0.15),
    0 0 0 16px rgba(102, 126, 234, 0.1) !important;
}

.fp-hero-title {
  font-size: 3.5rem;
  font-weight: 800;
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  background-clip: text;
  margin: 0;
  animation: fp-fade-in-up 1s ease-out 0.6s both;
}

.fp-hero-tagline {
  font-size: 1.3rem;
  color: var(--vp-c-text-2);
  margin: 0;
  animation: fp-fade-in-up 1s ease-out 0.8s both;
}

.fp-hero-actions {
  display: flex;
  gap: 1rem;
  flex-wrap: wrap;
  justify-content: center;
  animation: fp-fade-in-up 1s ease-out 1s both;
}

.fp-hero-actions .fp-action-btn {
  padding: 0.75rem 1.5rem;
  border-radius: 8px;
  text-decoration: none;
  font-weight: 500;
  transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
  display: inline-flex;
  align-items: center;
  gap: 0.5rem;
  position: relative;
  overflow: hidden;
}

.fp-hero-actions .fp-action-btn::before {
  content: '';
  position: absolute;
  top: 50%;
  left: 50%;
  width: 0;
  height: 0;
  border-radius: 50%;
  background: rgba(255, 255, 255, 0.2);
  transform: translate(-50%, -50%);
  transition: width 0.6s ease, height 0.6s ease;
}

.fp-hero-actions .fp-action-btn:hover::before {
  width: 300px;
  height: 300px;
}

.fp-hero-actions .fp-action-btn span {
  position: relative;
  z-index: 1;
}

.fp-hero-actions .fp-action-primary {
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  color: white;
  box-shadow: 0 4px 15px rgba(102, 126, 234, 0.3);
}

.fp-hero-actions .fp-action-primary:hover {
  transform: translateY(-2px);
  box-shadow: 0 6px 25px rgba(102, 126, 234, 0.4);
}

.fp-hero-actions .fp-action-default {
  background: rgba(102, 126, 234, 0.1);
  color: var(--vp-c-text-1);
  border: 1px solid rgba(102, 126, 234, 0.3);
}

.fp-hero-actions .fp-action-default:hover {
  background: rgba(102, 126, 234, 0.15);
  border-color: rgba(102, 126, 234, 0.5);
  transform: translateY(-1px);
}

.fp-highlight {
  position: relative;
  background-size: cover;
  background-position: center center;
  background-repeat: no-repeat;
  background-attachment: scroll;
  background-color: #ffffff;
}

.dark .fp-highlight {
  background-color: #0d1117;
}

.fp-highlight::before {
  content: '';
  position: absolute;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background: rgba(255, 255, 255, 0.6);
  z-index: 1;
}

.dark .fp-highlight::before {
  background: rgba(13, 17, 23, 0.6);
}

.fp-highlight-content {
  position: relative;
  z-index: 2;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  text-align: center;
  height: 100%;
  max-height: none;
  overflow: visible;
  padding: 1rem;
  box-sizing: border-box;
}

.fp-highlight-header {
  font-size: clamp(1.8rem, 4vw, 2.2rem);
  font-weight: 700;
  margin-bottom: 0.8rem;
  color: var(--vp-c-text-1);
}

.fp-highlight-desc {
  font-size: clamp(0.95rem, 2vw, 1rem);
  color: var(--vp-c-text-2);
  margin-bottom: 2rem;
  max-width: 90%;
  line-height: 1.5;
}

.fp-features {
  display: flex;
  flex-wrap: wrap;
  justify-content: center;
  align-items: stretch;
  gap: clamp(1rem, 2vw, 1.5rem);
  width: 100%;
  max-width: 1100px;
  margin: 0 auto;
}

.fp-features .fp-feature {
  flex: 0 0 auto;
  width: clamp(240px, 20vw, 280px);
  height: clamp(180px, 16vw, 220px);
  min-width: 240px;
  max-width: 280px;
  min-height: 180px;
  max-height: 220px;
}

.fp-features .fp-feature:nth-child(n+3) {
  width: clamp(240px, 20vw, 280px);
  height: clamp(180px, 16vw, 220px);
}

.fp-feature {
  background: rgba(255, 255, 255, 0.25) !important;
  backdrop-filter: blur(20px) !important;
  -webkit-backdrop-filter: blur(20px) !important;
  border: 1px solid rgba(255, 255, 255, 0.3) !important;
  padding: clamp(1rem, 2vw, 1.5rem);
  border-radius: 16px !important;
  box-shadow: 
    0 8px 32px rgba(0, 0, 0, 0.1),
    0 2px 8px rgba(0, 0, 0, 0.05),
    inset 0 1px 0 rgba(255, 255, 255, 0.4) !important;
  transition: all 0.4s cubic-bezier(0.4, 0, 0.2, 1) !important;
  opacity: 0;
  animation: fp-fade-in-up 0.6s ease-out forwards;
  position: relative;
  overflow: hidden;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: space-between;
  text-align: center;
  box-sizing: border-box;
}

.dark .fp-feature {
  background: rgba(255, 255, 255, 0.1) !important;
  border: 1px solid rgba(255, 255, 255, 0.15) !important;
  box-shadow: 
    0 8px 32px rgba(0, 0, 0, 0.3),
    0 2px 8px rgba(0, 0, 0, 0.2),
    inset 0 1px 0 rgba(255, 255, 255, 0.1) !important;
}

.fp-feature::before {
  content: '';
  position: absolute;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background: linear-gradient(135deg, 
    rgba(255, 255, 255, 0.1) 0%, 
    rgba(255, 255, 255, 0.05) 50%, 
    rgba(255, 255, 255, 0.1) 100%);
  border-radius: 16px;
  z-index: -1;
  opacity: 0;
  transition: opacity 0.3s ease;
}

.dark .fp-feature::before {
  background: linear-gradient(135deg, 
    rgba(255, 255, 255, 0.05) 0%, 
    rgba(255, 255, 255, 0.02) 50%, 
    rgba(255, 255, 255, 0.05) 100%);
}

.fp-feature:hover {
  transform: translateY(-8px) scale(1.02) !important;
  box-shadow: 
    0 20px 40px rgba(0, 0, 0, 0.15),
    0 8px 16px rgba(0, 0, 0, 0.1),
    inset 0 1px 0 rgba(255, 255, 255, 0.6) !important;
  border-color: rgba(255, 255, 255, 0.4) !important;
  background: rgba(255, 255, 255, 0.35) !important;
}

.dark .fp-feature:hover {
  box-shadow: 
    0 20px 40px rgba(0, 0, 0, 0.4),
    0 8px 16px rgba(0, 0, 0, 0.3),
    inset 0 1px 0 rgba(255, 255, 255, 0.2) !important;
  border-color: rgba(255, 255, 255, 0.25) !important;
  background: rgba(255, 255, 255, 0.15) !important;
}

.fp-feature:hover::before {
  opacity: 1;
}

.fp-section.active .fp-feature:nth-child(1) { animation-delay: 0.1s; }
.fp-section.active .fp-feature:nth-child(2) { animation-delay: 0.2s; }
.fp-section.active .fp-feature:nth-child(3) { animation-delay: 0.3s; }
.fp-section.active .fp-feature:nth-child(4) { animation-delay: 0.4s; }
.fp-section.active .fp-feature:nth-child(5) { animation-delay: 0.5s; }
.fp-section.active .fp-feature:nth-child(6) { animation-delay: 0.6s; }

.fp-feature-icon {
  font-size: clamp(1.8rem, 3vw, 2rem);
  margin-bottom: clamp(0.5rem, 1vw, 0.7rem);
  color: #667eea;
  flex-shrink: 0;
}

.fp-feature-title {
  font-size: clamp(0.95rem, 1.8vw, 1.05rem);
  font-weight: 600;
  margin-bottom: clamp(0.3rem, 0.5vw, 0.4rem);
  color: var(--vp-c-text-1);
  line-height: 1.2;
  flex-shrink: 0;
  text-align: center;
}

.fp-feature-details {
  font-size: clamp(0.8rem, 1.4vw, 0.85rem);
  color: var(--vp-c-text-2);
  line-height: 1.3;
  flex-grow: 1;
  display: flex;
  align-items: center;
  justify-content: center;
  text-align: center;
  hyphens: auto;
  word-wrap: break-word;
}

@keyframes fp-fade-in {
  0% {
    opacity: 0;
    transform: scale(0.9);
  }
  100% {
    opacity: 1;
    transform: scale(1);
  }
}

@keyframes fp-gentle-float {
  0%, 100% {
    transform: translateY(0);
  }
  50% {
    transform: translateY(-10px);
  }
}

@keyframes fp-glow-pulse {
  0%, 100% {
    opacity: 0.5;
    transform: translate(-50%, -50%) scale(1);
  }
  50% {
    opacity: 0.8;
    transform: translate(-50%, -50%) scale(1.1);
  }
}

@keyframes fp-fade-in-up {
  from {
    opacity: 0;
    transform: translateY(30px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}

@media (max-width: 1024px) {
  .fp-features .fp-feature {
    width: clamp(220px, 22vw, 260px);
    height: clamp(170px, 17vw, 200px);
    min-width: 220px;
    max-width: 260px;
    min-height: 170px;
    max-height: 200px;
  }
  
  .fp-features .fp-feature:nth-child(n+3) {
    width: clamp(220px, 22vw, 260px);
    height: clamp(170px, 17vw, 200px);
  }
}

@media (max-width: 768px) {
  #fp-nav {
    right: 15px;
    gap: 12px;
  }
  
  #fp-nav .fp-nav-item {
    width: 10px;
    height: 10px;
  }
  
  .fp-hero-title {
    font-size: 2.5rem;
  }
  
  .fp-hero-image {
    width: 220px !important;
    height: 220px !important;
    padding: 30px !important;
  }
  
  .fp-hero-image img {
    max-width: 160px !important;
    max-height: 160px !important;
  }
  
  .fp-hero-image::before {
    width: 260px;
    height: 260px;
  }
  
  .fp-features .fp-feature {
    width: clamp(280px, 80vw, 350px);
    height: clamp(160px, 20vw, 190px);
    min-width: 280px;
    max-width: 350px;
    min-height: 160px;
    max-height: 190px;
  }
  
  .fp-features .fp-feature:nth-child(n+3) {
    width: clamp(280px, 80vw, 350px);
    height: clamp(160px, 20vw, 190px);
  }
}

@media (max-width: 480px) {
  #fp-nav {
    display: none;
  }
  
  .fp-hero-title {
    font-size: 2rem;
  }
  
  .fp-hero-tagline {
    font-size: 1.1rem;
  }
  
  .fp-hero-image {
    width: 180px !important;
    height: 180px !important;
    padding: 25px !important;
  }
  
  .fp-hero-image img {
    max-width: 130px !important;
    max-height: 130px !important;
  }
  
  .fp-hero-image::before {
    width: 220px;
    height: 220px;
  }
  
  .fp-hero-actions {
    flex-direction: column;
    width: 100%;
  }
  
  .fp-hero-actions .fp-action-btn {
    width: 100%;
    justify-content: center;
  }
  
  .fp-highlight-content {
    padding: 0.5rem;
  }
  
  .fp-features .fp-feature {
    width: clamp(260px, 85vw, 320px);
    height: clamp(150px, 18vw, 180px);
    min-width: 260px;
    max-width: 320px;
    min-height: 150px;
    max-height: 180px;
  }
  
  .fp-features .fp-feature:nth-child(n+3) {
    width: clamp(260px, 85vw, 320px);
    height: clamp(150px, 18vw, 180px);
  }
}

.theme-hope-content .hero,
.theme-hope-content .vp-hero,
.theme-hope-content .vp-hero-info,
.theme-hope-content .vp-highlight,
.theme-hope-content .vp-features,
.theme-hope-content .vp-feature-wrapper,
.vp-footer,
footer,
.footer-wrapper,
.page-meta,
.page-nav {
  display: none !important;
  visibility: hidden !important;
  opacity: 0 !important;
  height: 0 !important;
  overflow: hidden !important;
  position: absolute !important;
  left: -9999px !important;
}

.theme-hope-content,
main,
.vp-page,
.vp-article-wrapper {
  overflow: hidden !important;
  height: 100vh !important;
}
</style>

<script setup>
import { onMounted, onUnmounted, ref, nextTick } from 'vue'

const currentSection = ref(0)
const isScrolling = ref(false)
const touchStartY = ref(0)
const totalSections = ref(0)

onMounted(() => {
  const hideOriginalContent = () => {
    const style = document.createElement('style')
    style.id = 'fp-mounted-style'
    style.innerHTML = `
      body > *:not(#fullpage-wrapper):not(#fp-nav):not(.navbar) {
        display: none !important;
      }
      body {
        background: ${document.documentElement.classList.contains('dark') ? '#0d1117' : '#ffffff'} !important;
      }
    `
    if (!document.getElementById('fp-mounted-style')) {
      document.head.appendChild(style)
    }
  }
  
  hideOriginalContent()
  
  nextTick(() => {
    if (document.readyState === 'complete') {
      initFullPage()
    } else {
      window.addEventListener('load', initFullPage)
    }
    
    setTimeout(() => {
      if (!document.getElementById('fullpage-wrapper')) {
        initFullPage()
      }
    }, 50)
  })
})

onUnmounted(() => {
  cleanupFullPage()
})

function getBaseUrl() {
  const currentPath = window.location.pathname
  
  if (currentPath.includes('/projects/xiaozhi/')) {
    return '/projects/xiaozhi'
  }
  
  const baseElement = document.querySelector('base')
  if (baseElement && baseElement.href) {
    try {
      const baseUrl = new URL(baseElement.href).pathname
      return baseUrl.endsWith('/') ? baseUrl.slice(0, -1) : baseUrl
    } catch (e) {
      console.warn('无法解析base标签URL:', e)
    }
  }
  
  if (window.__VUEPRESS_SSR_CONTEXT__ && window.__VUEPRESS_SSR_CONTEXT__.base) {
    const base = window.__VUEPRESS_SSR_CONTEXT__.base
    return base.endsWith('/') ? base.slice(0, -1) : base
  }
  
  return ''
}

function initFullPage() {
  console.log('初始化全屏滚动...')
  
  const baseUrl = getBaseUrl()
  console.log('检测到的基础URL:', baseUrl)
  
  const emergencyStyle = document.createElement('style')
  emergencyStyle.id = 'fp-emergency-style'
  emergencyStyle.innerHTML = `
    .vp-hero, .hero, .vp-highlight, .vp-features, 
    .vp-feature-wrapper, .page-meta, .page-nav, 
    .vp-footer, footer, .footer-wrapper {
      display: none !important;
      visibility: hidden !important;
      opacity: 0 !important;
      height: 0 !important;
      overflow: hidden !important;
      position: absolute !important;
      left: -99999px !important;
    }
    body { 
      overflow: hidden !important; 
      background: ${document.documentElement.classList.contains('dark') ? '#0d1117' : '#ffffff'} !important;
      margin: 0 !important;
      padding: 0 !important;
    }
    #fullpage-wrapper {
      position: fixed !important;
      top: 0 !important;
      left: 0 !important;
      width: 100vw !important;
      height: 100vh !important;
      z-index: 999 !important;
      display: block !important;
      visibility: visible !important;
    }
  `
  if (!document.getElementById('fp-emergency-style')) {
    document.head.appendChild(emergencyStyle)
  }
  
  const originalElements = document.querySelectorAll('.hero, .vp-hero, .vp-highlight, .vp-features, .vp-footer, footer')
  originalElements.forEach(el => {
    if (el) {
      el.style.display = 'none'
      el.style.visibility = 'hidden'
      el.style.height = '0'
      el.style.overflow = 'hidden'
    }
  })
  
  const mainContent = document.querySelector('.theme-hope-content') || document.querySelector('main') || document.querySelector('.vp-page')
  if (mainContent) {
    mainContent.innerHTML = ''
    mainContent.style.overflow = 'hidden'
    mainContent.style.height = '100vh'
  }
  
  const wrapper = document.createElement('div')
  wrapper.id = 'fullpage-wrapper'
  
  const container = document.createElement('div')
  container.className = 'fp-container'
  container.style.transform = 'translateY(0)'
  
  const heroSection = createHeroSection(baseUrl)
  container.appendChild(heroSection)
  
  const highlights = [
    {
      title: 'SF32芯片核心',
      desc: '基于先进的SF32芯片平台，为小智提供强大的图形能力和低功耗优势',
      bgImage: 'https://www.sifli.com/sites/default/files/2024-06/frontslider.jpg',
      features: [
        { icon: '🔧', title: '高性能处理器', details: 'ARM Cortex-M33内核，集成高性能2D/2.5D图形引擎' },
        { icon: '🔌', title: '丰富外设接口', details: '集成UART、SPI、I2C、ADC、PWM等多种外设' },
        { icon: '🔋', title: '超低功耗设计', details: '多种低功耗模式，休眠功耗仅2uA' },
        { icon: '🛡️', title: '硬件级安全', details: '集成AES、HASH、CRC加速器等硬件加密引擎' },
        { icon: '🔨', title: '完善工具链', details: '提供完整的SDK和开发工具，支持多种IDE环境' }
      ]
    },
    {
      title: '⚡ 易于开发部署',
      desc: '提供完整的开发框架和工具链，全方位支持开发者快速构建智能语音应用',
      bgImage: 'https://theme-hope-assets.vuejs.press/bg/9-light.svg',
      features: [
        { icon: '📦', title: '模块化架构', details: '采用分层模块化设计，支持灵活配置和自定义扩展' },
        { icon: '💻', title: '丰富的SDK工具包', details: '提供完整的SDK和API，详细的接口文档' },
        { icon: '📄', title: '海量例程代码', details: '包含100+示例代码和完整应用案例' },
        { icon: '☁️', title: '云端服务集成', details: '支持与云端服务主流平台集成' },
        { icon: '🔥', title: 'OTA无线升级', details: '支持固件和AI模型的空中升级' },
        { icon: '🛠', title: '专业调试工具', details: '提供性能分析、日志监控、错误诊断等调试工具' }
      ]
    },
    {
      title: '🌟 开源生态社区',
      desc: '完全开源的解决方案，拥有活跃的开发者社区和持续的技术创新',
      bgImage: 'https://theme-hope-assets.vuejs.press/bg/1-light.svg',
      features: [
        { icon: '📜', title: 'MIT开源许可', details: '基于MIT许可证完全开源，无版权限制' },
        { icon: '👥', title: '活跃开发社区', details: '完善的开发者社区，官方24小时技术支持' },
        { icon: '🔄', title: '持续版本更新', details: '定期发布新版本，持续优化性能' },
        { icon: '📚', title: '丰富技术资源', details: '详细的技术文档、视频教程、最佳实践分享' },
        { icon: '🤝', title: '开放贡献机制', details: '欢迎全球开发者贡献代码' }
      ]
    }
  ]
  
  highlights.forEach((highlight, index) => {
    const section = createHighlightSection(highlight, index + 1)
    container.appendChild(section)
  })
  
  totalSections.value = highlights.length + 1
  
  wrapper.appendChild(container)
  
  if (document.body.firstChild) {
    document.body.insertBefore(wrapper, document.body.firstChild)
  } else {
    document.body.appendChild(wrapper)
  }
  
  wrapper.style.cssText = `
    position: fixed !important;
    top: 0 !important;
    left: 0 !important;
    width: 100vw !important;
    height: 100vh !important;
    z-index: 999 !important;
    background: ${document.documentElement.classList.contains('dark') ? '#0d1117' : '#ffffff'} !important;
    overflow: hidden !important;
  `
  
  document.querySelectorAll('.vp-hero, .hero, .vp-highlight, .vp-features, .vp-feature-wrapper').forEach(el => {
    if (el && !el.closest('#fullpage-wrapper') && el.id !== 'fullpage-wrapper') {
      el.remove()
    }
  })
  
  document.body.style.background = 'var(--vp-c-bg)'
  
  createNavigation()
  
  bindEvents(baseUrl)
  
  activateSection(0)
  
  setTimeout(() => {
    const allElements = document.body.children
    for (let el of allElements) {
      if (el.id !== 'fullpage-wrapper' && 
          el.id !== 'fp-nav' && 
          !el.classList.contains('navbar')) {
        el.style.display = 'none'
        el.style.visibility = 'hidden'
        el.style.position = 'absolute'
        el.style.left = '-99999px'
      }
    }
    
    const fpWrapper = document.getElementById('fullpage-wrapper')
    if (fpWrapper) {
      fpWrapper.style.display = 'block'
      fpWrapper.style.visibility = 'visible'
      fpWrapper.style.opacity = '1'
      fpWrapper.style.position = 'fixed'
      fpWrapper.style.top = '0'
      fpWrapper.style.left = '0'
      fpWrapper.style.width = '100vw'
      fpWrapper.style.height = '100vh'
      fpWrapper.style.zIndex = '999'
      fpWrapper.style.background = document.documentElement.classList.contains('dark') ? '#0d1117' : '#ffffff'
    }
    
    document.body.style.background = document.documentElement.classList.contains('dark') ? '#0d1117' : '#ffffff'
  }, 200)
  
  console.log('全屏滚动初始化完成')
  
  const ensureHidden = setInterval(() => {
    document.querySelectorAll('.vp-hero, .hero, .vp-highlight').forEach(el => {
      if (el && !el.closest('#fullpage-wrapper')) {
        el.style.display = 'none'
        el.style.visibility = 'hidden'
        el.style.position = 'absolute'
        el.style.left = '-99999px'
      }
    })
  }, 100)
  
  setTimeout(() => clearInterval(ensureHidden), 5000)
}

function createHeroSection(baseUrl) {
  const section = document.createElement('div')
  section.className = 'fp-section fp-hero active'
  
  const logoSrc = `${baseUrl}/assets/image/xiaozhi-logo.svg`
  
  console.log('Logo路径:', logoSrc)
  
  section.innerHTML = `
    <div class="fp-section-content">
      <div class="fp-hero-content">
        <div class="fp-hero-image">
          <img src="${logoSrc}" alt="小智AI Logo - 智能语音平台" onload="console.log('Logo加载成功:', this.src)" onerror="console.error('Logo加载失败:', this.src)">
        </div>
        <h1 class="fp-hero-title">小智百科全书</h1>
        <p class="fp-hero-tagline">基于SF32平台 -- 构建全新语音交互体验</p>
        <div class="fp-hero-actions">
          <a href="./get-started/" class="fp-action-btn fp-action-primary">
            <span>💡</span> 快速入门
          </a>
          <a href="./architecture/" class="fp-action-btn fp-action-default">
            <span>🗃️</span> 架构设计
          </a>
          <a href="https://docs.sifli.com/projects/sdk/latest/sf32lb52x/index.html" class="fp-action-btn fp-action-default">
            <span>📖</span> SDK文档
          </a>
          <a href="https://bbs.sifli.com/" class="fp-action-btn fp-action-default">
            <span>💬</span> 社区论坛
          </a>
        </div>
      </div>
      <div class="fp-scroll-hint"></div>
    </div>
  `
  
  return section
}

function createHighlightSection(data, index) {
  const section = document.createElement('div')
  section.className = 'fp-section fp-highlight'
  
  section.style.backgroundImage = `url(${data.bgImage})`
  section.style.backgroundSize = 'cover'
  section.style.backgroundRepeat = 'no-repeat'
  section.style.backgroundPosition = 'center center'
  section.style.backgroundAttachment = 'scroll'
  
  const featuresHTML = data.features.map(feature => `
    <div class="fp-feature">
      <div class="fp-feature-icon">${feature.icon}</div>
      <h3 class="fp-feature-title">${feature.title}</h3>
      <p class="fp-feature-details">${feature.details}</p>
    </div>
  `).join('')
  
  section.innerHTML = `
    <div class="fp-section-content">
      <div class="fp-highlight-content">
        <h2 class="fp-highlight-header">${data.title}</h2>
        <p class="fp-highlight-desc">${data.desc}</p>
        <div class="fp-features">
          ${featuresHTML}
        </div>
      </div>
    </div>
  `
  
  return section
}

function createNavigation() {
  const nav = document.createElement('div')
  nav.id = 'fp-nav'
  
  const titles = ['主页', 'SF32芯片核心', '易于开发部署', '开源生态社区']
  
  for (let i = 0; i < totalSections.value; i++) {
    const item = document.createElement('div')
    item.className = 'fp-nav-item'
    item.setAttribute('data-tooltip', titles[i] || `板块 ${i + 1}`)
    if (i === 0) item.classList.add('active')
    
    item.addEventListener('click', () => {
      scrollToSection(i)
    })
    
    nav.appendChild(item)
  }
  
  document.body.appendChild(nav)
}

function bindEvents(baseUrl) {
  window.addEventListener('wheel', handleWheel, { passive: false })
  
  window.addEventListener('keydown', handleKeydown)
  
  window.addEventListener('touchstart', handleTouchStart, { passive: true })
  window.addEventListener('touchend', handleTouchEnd)
  
  const scrollHint = document.querySelector('.fp-scroll-hint')
  if (scrollHint) {
    scrollHint.addEventListener('click', () => {
      scrollToSection(1)
    })
  }
  
  const observer = new MutationObserver((mutations) => {
    mutations.forEach((mutation) => {
      if (mutation.type === 'attributes' && mutation.attributeName === 'class') {
        const isDark = document.documentElement.classList.contains('dark')
        const heroImg = document.querySelector('.fp-hero img')
        if (heroImg) {
          heroImg.src = `${baseUrl}/assets/image/xiaozhi-logo.svg`
        }
        const fpWrapper = document.getElementById('fullpage-wrapper')
        if (fpWrapper) {
          fpWrapper.style.background = isDark ? '#0d1117' : '#ffffff'
        }
        document.body.style.background = isDark ? '#0d1117' : '#ffffff'
      }
    })
  })
  
  observer.observe(document.documentElement, {
    attributes: true,
    attributeFilter: ['class']
  })
}

function handleWheel(e) {
  e.preventDefault()
  
  if (isScrolling.value) return
  
  const delta = Math.sign(e.deltaY)
  
  if (delta > 0) {
    if (currentSection.value < totalSections.value - 1) {
      scrollToSection(currentSection.value + 1)
    }
  } else if (delta < 0) {
    if (currentSection.value > 0) {
      scrollToSection(currentSection.value - 1)
    }
  }
}

function handleKeydown(e) {
  if (isScrolling.value) return
  
  switch(e.key) {
    case 'ArrowDown':
    case 'PageDown':
      e.preventDefault()
      if (currentSection.value < totalSections.value - 1) {
        scrollToSection(currentSection.value + 1)
      }
      break
    case 'ArrowUp':
    case 'PageUp':
      e.preventDefault()
      if (currentSection.value > 0) {
        scrollToSection(currentSection.value - 1)
      }
      break
    case 'Home':
      e.preventDefault()
      scrollToSection(0)
      break
    case 'End':
      e.preventDefault()
      scrollToSection(totalSections.value - 1)
      break
  }
}

function handleTouchStart(e) {
  touchStartY.value = e.touches[0].clientY
}

function handleTouchEnd(e) {
  if (isScrolling.value) return
  
  const touchEndY = e.changedTouches[0].clientY
  const diff = touchStartY.value - touchEndY
  
  if (Math.abs(diff) > 50) {
    if (diff > 0 && currentSection.value < totalSections.value - 1) {
      scrollToSection(currentSection.value + 1)
    } else if (diff < 0 && currentSection.value > 0) {
      scrollToSection(currentSection.value - 1)
    }
  }
}

function scrollToSection(index) {
  if (index < 0 || index >= totalSections.value) return
  if (index === currentSection.value) return
  if (isScrolling.value) return
  
  console.log(`滚动到板块 ${index}`)
  
  isScrolling.value = true
  currentSection.value = index
  
  const container = document.querySelector('.fp-container')
  if (container) {
    container.style.transform = `translateY(-${index * 100}vh)`
  }
  
  activateSection(index)
  
  setTimeout(() => {
    isScrolling.value = false
  }, 1000)
}

function activateSection(index) {
  const sections = document.querySelectorAll('.fp-section')
  sections.forEach((section, i) => {
    if (i === index) {
      section.classList.add('active')
    } else {
      section.classList.remove('active')
    }
  })
  
  const navItems = document.querySelectorAll('.fp-nav-item')
  navItems.forEach((item, i) => {
    if (i === index) {
      item.classList.add('active')
    } else {
      item.classList.remove('active')
    }
  })
  
  const scrollHint = document.querySelector('.fp-scroll-hint')
  if (scrollHint) {
    scrollHint.style.display = index === 0 ? 'block' : 'none'
  }
}

function cleanupFullPage() {
  document.body.style.overflow = ''
  document.documentElement.style.overflow = ''
  
  window.removeEventListener('wheel', handleWheel)
  window.removeEventListener('keydown', handleKeydown)
  window.removeEventListener('touchstart', handleTouchStart)
  window.removeEventListener('touchend', handleTouchEnd)
  
  const wrapper = document.getElementById('fullpage-wrapper')
  if (wrapper) wrapper.remove()
  
  const nav = document.getElementById('fp-nav')
  if (nav) nav.remove()
}

if (typeof document !== 'undefined') {
  document.addEventListener('DOMContentLoaded', () => {
    document.body.style.overflow = 'hidden'
    document.documentElement.style.overflow = 'hidden'
  })
}
</script>