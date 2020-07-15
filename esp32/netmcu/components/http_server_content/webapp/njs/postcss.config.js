const isProduction = process.env.buildTarget === "PROD";
const useTailwind = process.env.useTailwind === "yes";

module.exports = {
  plugins: [
    useTailwind && require('tailwindcss'),
    useTailwind && isProduction && require('@fullhuman/postcss-purgecss')({
      content: [
        './**/**/*.html',
        './**/**/*.svelte'
      ],
    
      whitelistPatterns: [/svelte-/],
    
      defaultExtractor: content => content.match(/[\w-/:]+(?<!:)/g) || []
    }),
  ]
};
