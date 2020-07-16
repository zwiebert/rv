// rollup.config.js
import json from '@rollup/plugin-json';
import { terser } from 'rollup-plugin-terser';
import strip from '@rollup/plugin-strip';
import { eslint } from "rollup-plugin-eslint";
import svelte from 'rollup-plugin-svelte-hot';
import resolve from '@rollup/plugin-node-resolve';
import sveltePreprocess from 'svelte-preprocess';
import { watch } from 'rollup';

export const isProduction = process.env.buildTarget === "PROD";
const useTailwind = process.env.useTailwind === "yes";

let hot = !isProduction && !!watch;

export default {
  onwarn(warning, rollupWarn) {
    if (warning.code !== 'CIRCULAR_DEPENDENCY') {
      rollupWarn(warning);
    }
  },
  input: useTailwind ? 'src/main_tailwind.js' : 'src/main.js',
  output: [...!isProduction ? [ {
    file: 'build_dev/wapp.js',
    sourcemap: true,
    format: 'iife',
    name: 'wapp',
    // sourcemapPathTransform: relativePath => {      return relativePath.substr(2);},
    plugins: [
    ]
  }] : [ {
    file: 'build/wapp.js',
    format: 'iife',
    name: 'wapp',
    sourcemap: true,
    sourcemapPathTransform: relativePath => {
      // will transform e.g. "src/main.js" -> "main.js"
      return relativePath.substr(2);
    },
    plugins: [
      terser()
    ]
  }]],
  plugins: [
    json(),
    ...isProduction ? [
      strip({
        functions: ['testing.*', 'testing_*', 'appDebug.*', 'console.*', 'assert.*'],
        labels: ['testing'],
        sourceMap: true
      })] : [],
    eslint(),
    svelte({

      // Use `hot: true` to use default options (as follow).
      //
      // Set `hot: false` to completely disable HMR shenanigans (you need this
      // for `npm run build`, for example).
      //
      hot: hot && {
        // Prevent preserving local component state
        noPreserveState: false,

        // If this string appears anywhere in your component's code, then local
        // state won't be preserved, even when noPreserveState is false
        noPreserveStateKey: '@!hmr',

        // Prevent doing a full reload on next HMR update after fatal error
        noReload: false,

        // Try to recover after runtime errors in component init
        optimistic: false,

        // --- Advanced ---

        // By default, when the hot option is enabled, the `css` option of this
        // plugin (same option as official plugin) will be changed to `false`,
        // because extracting CSS doesn't work with HMR currently. You can use
        // this option to disable this behaviour if it cause problems with your
        // setup.
        noDisableCss: true,

        // When you change only the <style> part of a component, then only the
        // CSS will be reinjected. Existing component instances won't be
        // recreated. Set `false` to force recreation.
        injectCss: true,

        // Delay to mitigate FOUC (flash of unstyled content) when a component
        // is updated, before the new version with the new CSS is loaded.
        cssEjectDelay: 100,

        // Prevent adding an HMR accept handler to components with
        // accessors option to true, or to components with named exports
        // (from <script context="module">). This have the effect of
        // recreating the consumer of those components, instead of the
        // component themselves, on HMR updates. This might be needed to
        // reflect changes to accessors / named exports in the parents,
        // depending on how you use them.
        acceptAccessors: false,
        acceptNamedExports: false,

        // Set true to enable support for Nollup (note: when not specified, this
        // is automatically detected based on the NOLLUP env variable)
        nollup: false,
      },

      dev: !isProduction,
      //emitCss: true,
      ...(true && {
      css: css => { css.write(isProduction ? 'build/wapp.css' : 'build_dev/wapp.css'); }
      }),
      onwarn: (warning, handler) => {
        // e.g. don't warn on <marquee> elements, cos they're cool
        if (warning.code === 'a11y-no-onchange') return;

        // let Rollup handle all other warnings normally
        handler(warning);
      },
      preprocess: sveltePreprocess({postcss: true}),

    }),
    // If you have external dependencies installed from
    // npm, you'll most likely need these plugins. In
    // some cases you'll need additional configuration -
    // consult the documentation for details:
    // https://github.com/rollup/plugins/tree/master/packages/commonjs
    resolve({
      browser: true,
      dedupe: ['svelte']
    })
  ]
};



function serve() {
	let started = false;

	return {
		writeBundle() {
			if (!started) {
				started = true;

				require('child_process').spawn('npm', ['run', 'start', '--', '--dev'], {
					stdio: ['ignore', 'inherit', 'inherit'],
					shell: true
				});
			}
		}
	};
}
