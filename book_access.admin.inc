<?php
// $Id$

/**
 * @file
 *
 * Administration interface for Book access module.
 */

/**
 * Book access configuration page.
 *
 */
function book_access_permissions_form($form_state, $node) {
  $default = array(
    'view' => array(),
    'update' => array(),
    'delete' => array(),
  );
  $form = array(
    '#node' => $node,
    '#rids' => array(),
    '#uids' => array(),
    '#theme' => array('book_access_permissions_form'),
    '#tree' => TRUE,
  );
  $grants = array(
    'grant_view' => 'view',
    'grant_update' => 'update',
    'grant_delete' => 'delete',
  );
  $options = array();
  $result = db_result(
    db_query_range(
      "SELECT 1 FROM {book_access_role} where nid = %d", $node->book['bid'],
      0, 1
    )
  );

  drupal_set_message(
    t('Node access tables must be <a href="@rebuild">rebuilt</a> when these settings are changed. This may take a few moments.',
      array('@rebuild' => url('admin/content/node-settings/rebuild', array('query' => array('destination' => 'node/' . $node->book['bid'] . '/book_access'))))
    ),
    'warning',
    FALSE
  );

  // If there are no grants, then use some safe defaults; it only makes
  // sense when a new book is created.
  if (!$result) {
    $settings = array_filter(
      variable_get('book_access_default_roles_access', array())
    );

    foreach ($grants as $grant => $var) {
      if (isset($settings[$var])) {
        $default[$var] = array(DRUPAL_ANONYMOUS_RID, DRUPAL_AUTHENTICATED_RID);
      }
    }
  }
  else {
    $result = db_query(
      "SELECT * FROM {book_access_role} where nid = %d", $node->book['bid']
    );

    while ($book_access = db_fetch_object($result)) {
      foreach ($grants as $grant => $var) {
        if ($book_access->$grant) {
          $default[$var][] = $book_access->rid;
        }
      }
    }
  }

  foreach (user_roles() as $rid => $name) {
    $form['#rids'][] = $rid;
    $options[$rid] = '';

    $form['roles']['names'][$rid] = array(
      '#value' => $name,
    );
  }

  if (!empty($options)) {
    $bool = count($options) > 1;

    $form['roles']['view'] = array(
      '#type' => 'checkboxes',
      '#options' => $options,
      '#default_value' => $default['view'],
      '#checkall' => $bool ? 'book-access-view' : FALSE,
    );

    $form['roles']['update'] = array(
      '#type' => 'checkboxes',
      '#options' => $options,
      '#default_value' => $default['update'],
      '#checkall' => $bool ? 'book-access-update' : FALSE,
    );

    $form['roles']['delete'] = array(
      '#type' => 'checkboxes',
      '#options' => $options,
      '#default_value' => $default['delete'],
      '#checkall' => $bool ? 'book-access-delete' : FALSE,
    );
  }

  $default = array(
    'view' => array(),
    'update' => array(),
    'delete' => array(),
  );
  $options = array();
  $result = db_query("SELECT * FROM {book_access_user} where nid = %d", $node->book['bid']);

  while ($book_access = db_fetch_object($result)) {
    $uid = $book_access->uid;

    $form['#uids'][] = $uid;
    $options[$uid] = '';

    if ($user = user_load($uid)) {
      $name = $user->name;
    }
    else {
      $name = t('User ID %id', array('%id' => $uid));
    }

    $form['users']['names'][$uid] = array(
      '#value' => $name,
    );

    foreach ($grants as $grant => $var) {
      if (!empty($book_access->$grant)) {
        $default[$var][] = $uid;
      }
    }
  }

  if (!empty($options)) {
    $bool = count($options) > 1;

    $form['users']['view'] = array(
      '#type' => 'checkboxes',
      '#options' => $options,
      '#default_value' => $default['view'],
      '#checkall' => $bool ? 'book-access-view' : FALSE,
    );

    $form['users']['update'] = array(
      '#type' => 'checkboxes',
      '#options' => $options,
      '#default_value' => $default['update'],
      '#checkall' => $bool ? 'book-access-update' : FALSE,
    );

    $form['users']['delete'] = array(
      '#type' => 'checkboxes',
      '#options' => $options,
      '#default_value' => $default['delete'],
      '#checkall' => $bool ? 'book-access-delete' : FALSE,
    );
  }

  $form['user']['username'] = array(
    '#type' => 'textfield',
    '#size' => 20,
    '#autocomplete_path' => 'user/autocomplete',
  );

  $settings = array_filter(
    variable_get('book_access_default_users_access', array())
  );

  $form['user']['view'] = array(
    '#type' => 'checkbox',
    '#default_value' => isset($settings['view']),
  );

  $form['user']['update'] = array(
    '#type' => 'checkbox',
    '#default_value' => isset($settings['update']),
  );

  $form['user']['delete'] = array(
    '#type' => 'checkbox',
    '#default_value' => isset($settings['delete']),
  );

  $form['user']['add_user'] = array(
    '#type'  => 'submit',
    '#value' => t('Add user'),
    '#submit' => array('book_access_add_user_submit'),
  );

  $form['submit'] = array(
    '#type'  => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

/**
 * Submission callback to add a new user to the permissions table.
 */
function book_access_add_user_submit($form, &$form_state) {
  $bid = $form['#node']->book['bid'];
  $grants = array(
    'grant_view' => 'view',
    'grant_update' => 'update',
    'grant_delete' => 'delete',
  );
  $username = $form_state['values']['user']['username'];

  if (!($user = user_load(array('name' => $username)))) {
    drupal_set_message(
      t('The user %user does not exist.', array('%user' => $username)),
      'error'
    );
  }
  else {
    $bool = db_result(
      db_query_range(
        "SELECT 1 FROM {book_access_user} WHERE nid = %d AND uid = %d",
        $bid, $user->uid, 0, 1
      )
    );
    $row = new stdClass();
    $row->nid = $bid;
    $row->uid = $user->uid;

    foreach ($grants as $grant => $var) {
      $row->$grant = $form_state['values']['user'][$var];
    }

    drupal_write_record('book_access_user', $row, $bool ? array('nid', 'uid') : array());

    drupal_set_message(t('The settings have been changed.'));
  }

  $form_state['redirect'] = 'node/' . $bid . '/book_access';
}

/**
 * Submission callback for the administration settings.
 */
function book_access_permissions_form_submit($form, &$form_state) {
  $bid = $form['#node']->book['bid'];
  $grants = array(
    'grant_view' => 'view',
    'grant_update' => 'update',
    'grant_delete' => 'delete',
  );

  $row = new stdClass();
  $row->nid = $bid;

  db_query("DELETE FROM {book_access_role} WHERE nid = %d", $bid);

  foreach ($form['#rids'] as $rid) {
    $row->rid = $rid;

    foreach ($grants as $grant => $var) {
      $row->$grant = !empty($form_state['values']['roles'][$var][$rid]);
    }

    drupal_write_record('book_access_role', $row);
  }

  foreach ($form['#uids'] as $uid) {
    $bool = db_result(
      db_query_range(
        "SELECT 1 FROM {book_access_user} WHERE nid = %d AND uid = %d",
        $bid, $uid, 0, 1
      )
    );

    $row->uid = $uid;

    foreach ($grants as $grant => $var) {
      $row->$grant = !empty($form_state['values']['users'][$var][$uid]);
    }

    drupal_write_record(
      'book_access_user', $row, $bool ? array('nid', 'uid') : array()
    );
  }
}

/**
 * Show the confirmation form to remove the permissions a user have for a
 * book.
 */
function book_access_remove_user_permissions($form_state, $node, $user) {
  $form = array(
    '#bid' => $node->book['bid'],
    '#uid' => $user->uid,
  );

  if ($top_node = node_load($node->book['bid'])) {
    $title = $top_node->title;
  }
  else {
    $title = '#' . $node->book['bid'];
  }

  return confirm_form(
    $form,
    t('Are you sure you want to remove the user %user permissions for the book %title?', array('%user' => $user->name, '%title' => $title)),
    'node/' . $node->book['bid'] . '/book_access',
     t('This action cannot be undone.'), t('Remove')
  );
}

function book_access_remove_user_permissions_submit($form, &$form_state) {
  db_query(
    "DELETE FROM {book_access_user} WHERE nid = %d AND uid = %d",
    $form['#bid'], $form['#uid']
  );

  drupal_set_message(t('The settings have been changed.'));

  $form_state['redirect'] = 'node/' . $form['#bid'] . '/book_access';
}

/**
 * Theme the permission tab added to each book.
 */
function theme_book_access_permissions_form($form) {
  $has_names = isset($form['roles']['names']) && is_array($form['roles']['names']);
  $header = array();
  $rows = array();

  if ($has_names) {
    foreach (element_children($form['roles']['names']) as $key) {
      $row = array();

      $row[] = drupal_render($form['roles']['names'][$key]);
      $row[] = drupal_render($form['roles']['view'][$key]);
      $row[] = drupal_render($form['roles']['update'][$key]);
      $row[] = drupal_render($form['roles']['delete'][$key]);
      $row[] = ' ';

      $rows[] = $row;
    }
  }

  $has_names = isset($form['users']['names']) && is_array($form['users']['names']);

  if ($has_names) {
    foreach (element_children($form['users']['names']) as $key) {
      $row = array();

      $row[] = drupal_render($form['users']['names'][$key]);
      $row[] = drupal_render($form['users']['view'][$key]);
      $row[] = drupal_render($form['users']['update'][$key]);
      $row[] = drupal_render($form['users']['delete'][$key]);
      $row[] = l(t('delete'), "book_access/delete/user_permission/{$form['#node']->nid}/$key");

      $rows[] = $row;
    }
  }

  $rows[] = array(
    drupal_render($form['user']['username']),
    drupal_render($form['user']['view']),
    drupal_render($form['user']['update']),
    drupal_render($form['user']['delete']),
    drupal_render($form['user']['add_user']),
  );

  $header[] = t('Role/user');
  $header[] = array(
    'data' => t('View this book'),
    'class' => 'book-access-view',
  );
  $header[] = array(
    'data' => t('Edit pages'),
    'class' => 'book-access-update',
  );
  $header[] = array(
    'data' => t('Delete pages'),
    'class' => 'book-access-delete',
  );
  $header[] = t('Operations');

  return theme('table', $header, $rows) . drupal_render($form);
}
