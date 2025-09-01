import { defineUserConfig } from "vuepress";

import theme from "./theme.js";

export default defineUserConfig({
  base: "/projects/xiaozhi/",

  locales: {
    "/en/": {
      lang: "en-US",
      title: "Xiaozhi Encyclopedia",
      description: "Xiaozhi Encyclopedia based on SF32",
    },
    "/": {
      lang: "zh-CN",
      title: "小智百科全书",
      description: "基于SF32的小智百科全书",
    },
  },

  theme,

  head: [
    [
      'script',
      {},
      `
      var _hmt = _hmt || [];
      (function() {
        var hm = document.createElement(\"script\");
        hm.src = \"https://hm.baidu.com/hm.js?b12a52eecef6bedee8b8e2d510346a6e\";
        var s = document.getElementsByTagName(\"script\")[0]; 
        s.parentNode.insertBefore(hm, s);
      })();
      `
    ]
  ],
  // Enable it with pwa
  // shouldPrefetch: false,
});
