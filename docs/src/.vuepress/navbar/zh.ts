import { navbar } from "vuepress-theme-hope";

export const zhNavbar = navbar([
  "/",
  {
    text: "快速入门",
    icon: "lightbulb",
    prefix: "/get-started/",
    children: [
      "yellow_mountain/README.md",
    ],
  },
  {
    text: "源码构建",
    icon: "code",
    link: "/source-build/",
  }
]);
