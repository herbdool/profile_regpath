<?php

/**
 * @file
 * Contains logic for modifying user registration forms.
 */

/*
 * Attach profile fields to form.
 */
function profile_regpath_attach_profile_fields(&$form, &$form_state, $form_id, $profile_types = NULL) {

  // Check to see if the form is being rebuilt after an AJAX request.
  // If so, we lost the $profile_types parameter, and need to reacquire it.
  $menu_item = menu_get_item();
  if (!$profile_types && ($menu_item['path'] == 'system/ajax' || $menu_item['path'] == 'file/ajax')) {
    $url = backdrop_parse_url($form['#action']);
    $path = explode('/', str_replace('/register', '', $url['path']));
    $path_key = end($path);
    $profile_types = _profile_regpath_load_regpath($path_key);
  }

  if ($profile_types != NULL) {

    // Prepare variables for roles.
    $user_roles = user_roles(TRUE);

    // Attach profile(s) to user/register form.
    foreach ($profile_types as $key => $value) {
      // Get profile object.
      $type_name = (string) $profile_types[$key]->profile_type;
      $profile_type = profile_get_types($type_name);

      // If this profile has not already been attached.
      if (empty($form_state['profiles'][$type_name])) {
        $profile = profile_create(array('type' => $type_name));
        $form_state['profiles'][$type_name] = $profile;

        $misc = unserialize($profile_types[$key]->misc);
        if (!$misc['fieldset_wrap']) {
          // Unwrap from fieldset (default in Profile module).
          $form['profile_' . $type_name] = array(
            '#type' => 'container',
          );
        }
        // Set Form API #weight attribute for profile.
        $form['profile_' . $type_name]['#weight'] = $profile_type->weight;

        // Attach custom confirmation message to form for later display.
        if (!empty($misc['confirmation_display'])) {
          $_SESSION['profile_regpath']['confirmation_message'] = filter_xss($misc['confirmation_message']);
        }
      }

      // Add appropriate user roles.
      $profile_roles = unserialize($value->roles);
      if (!empty($profile_roles)) {
        // Make sure roles still exist.
        $assign_roles = array_intersect_key($profile_roles, $form['account_settings']['roles']['#options']);
        $form['account_settings']['roles']['#default_value'] = $assign_roles;
      }

    }
    // Attach the profile fields via profile.
    // @todo Add test for AJAX fields after profile_attach_form() is called.
    profile_attach_form($form, $form_state);
  }
}
